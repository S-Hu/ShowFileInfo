/*
	ShowFileInfo
	This tool displays data of a binary file in assigned format.
	Type "ShowFileInfo /?" for usage.
	
	C++语言程序设计第3次作业
	by S.Hu
*/
#include<iostream>
#include<iomanip>
#include<string>
#include<fstream>

using namespace std;

ifstream myFile;//全局变量

struct typeWrapper {
	unsigned int typeSize;
	virtual void display(unsigned int num) = 0;
};
template<typename T>
struct Displayer:public typeWrapper {
	unsigned int position;
	Displayer(unsigned int pos) :position(pos) { typeSize = sizeof(T); }

	void display(unsigned int num) {
		myFile.seekg(position);
		int iPause = 0;
		for (unsigned int i = 0;i < num && !myFile.eof();++i) {
			T tmp;
			myFile.read(reinterpret_cast<char*>(&tmp), sizeof(T));
			cout << tmp << ' ';
			if (++iPause == 50) { system("pause>nul");iPause = 0; }
		}
	}
};

void ShowData(unsigned int position, const char *argList) {

	for (int i = 0;argList[i] != '\0';++i) {
		typeWrapper *myType;
		switch(argList[i]){
		case 'C':
			myType = new Displayer<char>(position);
			break;
		case 'I':
			myType = new Displayer<int>(position);
			break;
		case 'S':
			myType = new Displayer<short>(position);
			break;
		case 'U':
			myType = new Displayer<unsigned>(position);
			break;
		case 'L':
			myType = new Displayer<long>(position);
			break;
		case 'D':
			myType = new Displayer<double>(position);
			break;
		case 'F':
			myType = new Displayer<float>(position);
			break;
		case 'B':
			myType = new Displayer<bool>(position);
			break;
		case 'G':{
			myType = nullptr;
			break;
		}
		default:
			myType = nullptr;
			cerr <<"\nError: Invalid argument/option - \'"<< argList[i] << "\'. The byte will be skipped.\nType \"ShowFileInfo /?\" for usage.\n";
			break;
		}
		//接收数字
		int offset = 0;
		for (;(argList[i + 1] >= '0' && argList[i + 1] <= '9')|| argList[i + 1]=='-';) {
			++i;
			if (argList[i] == '-') { offset = unsigned(-1); break; }
			offset = offset * 10 + argList[i] - 48;
		}
		if (offset == 0)offset = 1;
		if (myType != nullptr) {
			myType->display(offset);
			position += offset*myType->typeSize;
			delete myType;
		}else position += offset;

	}
}

void ShowHex(unsigned int startByte) {
	ostream::fmtflags oldfmtflags = cout.flags();//保存旧flags
	cout << hex;//使用hex输出
	myFile.seekg(startByte);
	unsigned char myByte;
	myFile.read(reinterpret_cast<char*>(&myByte), 1);
	while (!myFile.eof()) {
		for (int row = 0;row < 25 && !myFile.eof();++row) {
			for (int col = 0;col < 20 && !myFile.eof();++col) {
				cout << setw(2) << setfill('0') << unsigned short int(myByte) << ' ';//格式化输出
				myFile.read(reinterpret_cast<char*>(&myByte), 1);
			}
			cout << endl;
		}
		system("pause>nul");
	}
	cout.flags(oldfmtflags);//恢复旧flags
}

int main(int argc,char *argv[]) {
	
	//显示帮助
	if (argc>=2&&!strcmp(argv[1],"/?")) { 
		cout << "\n\
    ShowFileInfo [/filetype] [dirve:][path]filename [startByte] [paraList]\n\n\
Description:\n\
    This tool displays data of a binary file in assigned format.\n\n\
Parameter List:\n\
    [/filetype]    To display specific information of a certain filetype.\n\
                   Only \"/BMP\" is accepted in this test version.\n\n\
    [dirve:][path]filename    Specifies the file to analize.\n\
                              You must assign one file.\n\n\
    [startByte]    Specifies the origin byte to analize.\n\
                   You can use either Oct or Hex numbers.(Note that Hex numbers\n\
                   must be started with 0x.) \n\n\
    [paraList]     Specifies the format to display data in.\n\
                   If not assigned, the data will be displayed in Hex.\n\
        C    To display 1 byte as Char.\n\
        I    To display 4 bytes as Integer.\n\
        S    To display 2 bytes as Short integer.\n\
        U    To display 4 bytes as Unsigned integer.\n\
        L    To display 8 bytes as Long integer.\n\
        D    To display 8 bytes as Double.\n\
        F    To display 4 bytes as Float.\n\
        B    To display 1 byte as Boolean.\n\
        G    To skip 1 byte.\n\
        #    These upper case letters may be followed by numbers indicating how\n\
             much of the following data will be interpreted in this type.\n\
             You may use '-' to indicate all of the following data to be\n\
             interpreted in this type.\n\
             For Example, \"C4\" is equivalent to \"CCCC\".\n\n\
    /?     Display this help message.\n\n\
Examples:\n\
    ShowFileInfo test.bmp\n\
    ShowFileInfo /BMP test.bmp\n\
    ShowFileInfo test.bmp 0x1e\n\
    ShowFileInfo test.bmp C2F2D4B-\n\
    ShowFileInfo test.bmp 0 C2UG4UI3S2U2I2U2\n";
		return 0;
	}

	//解析bmp
	if(argc>=3 && !(strcmp(argv[1],"/BMP") && strcmp(argv[1],"/bmp"))) { 
		myFile.open(argv[2], ios::binary | ios::in);
		if (myFile.rdstate() > 0) {
			cerr << "Error: Cannot open file." << endl;
			exit(1);
		}
		cout << "---BITMAP FILE HEADER---";
		cout << endl << "bfType=";ShowData(0x0, "C2");
		cout << endl << "bfSize=";ShowData(0x2, "U");
		cout << endl << "bfOffBits=";ShowData(0xa, "U");

		cout << endl << "---BITMAP INFO HEADER---";
		cout << endl << "biSize=";ShowData(0xe, "U");
		cout << endl << "biWidth=";ShowData(0x12, "I");
		cout << endl << "biHeight=";ShowData(0x16, "I");
		cout << endl << "biPlanes=";ShowData(0x1a, "S");
		cout << endl << "biBitCount=";ShowData(0x1c, "S");
		cout << endl << "biCompression=";ShowData(0x1e, "U");
		cout << endl << "biSizeImage=";ShowData(0x22, "U");
		cout << endl << "biXPelsPerMeter=";ShowData(0x26, "I");
		cout << endl << "biYPelsPerMeter=";ShowData(0x2a, "I");
		cout << endl << "biClrUsed=";ShowData(0x2e, "U");
		cout << endl << "biClrImportant=";ShowData(0x32, "U");
		myFile.close();
		return 0;
	}
	
	//缺少文件名
	if (argc == 1) {
		cerr << "The syntax of the command is incorrect.\n\
Type \"ShowFileInfo /?\" for usage,\n\
or input the path and filename of a file or drag it into the box: ";
		string fileName;
		getline(cin, fileName);
		if (fileName.size() == 0)return 0;//没有输入直接退出
		if (fileName.front() == '\"')fileName.erase(fileName.begin());//删去两端的引号
		if (fileName.back() == '\"')fileName.erase(fileName.end() - 1);
		myFile.open(fileName, ios::binary | ios::in);
	}
	else myFile.open(argv[1], ios::binary | ios::in);

	//检查文件是否打开成功
	if (myFile.rdstate() > 0) {
		cerr << "Error: Cannot open file." << endl;
		exit(1);
	}
	

	//匹配可选参数
	int startByte = 0;
	const char *paraList = "";
	if (argc >= 3) {
		if (argv[2][0] >= '0' && argv[2][0] <= '9') {//第一个字符为数字，匹配startByte
			if (argv[2][0] == '0' && argv[2][1] == 'x')startByte = strtol(argv[2], NULL, 16);//hex
			else startByte = atoi(argv[2]);//oct
			if (argc > 3) paraList = argv[3];//还有参数则匹配paraList
		}
		else paraList = argv[2];//第一个字符为字母，匹配paraList
	}
	if (paraList == "")ShowHex(startByte);//无paraList，显示Hex
	else ShowData(startByte, paraList);//有paraList
	
	myFile.close();
	return 0;
}

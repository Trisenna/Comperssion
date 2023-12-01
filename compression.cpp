#include "compression.h"

#include <QDir>
//binarystringtoint 函数将二进制字符串转换为整数。
int binarystringtoint(string binarystring) {
    int sum = 0;
    for (int i = 0; i < binarystring.size(); i++) {
        if (binarystring[i] == '1') {
            int j = pow(2, binarystring.size() - i - 1);
            sum += j;
        }
    }
    return sum;
}

//inttobinarystring 函数将整数转换为8位的二进制字符串。
string inttobinarystring(int value) {
    string binarystring;
    while (value > 0) {
        int r = value % 2;

        if (r == 1)binarystring.insert(0, 1, '1');
        else binarystring.insert(0, 1, '0');
        value = value / 2;
    }
    //不足的补零
    if (binarystring.size() < 8) {
        //在前方补零
        binarystring.insert(0, 8 - binarystring.size(), '0');
    }

    return binarystring;
}
//compare 函数是用于比较Huffman树节点权重的比较函数。
bool compare(HuffmanTreeNode* node1, HuffmanTreeNode* node2) {
    //比较权重
    return node1->Weight.second < node2->Weight.second;
}

//Compression 类是主要的压缩和解压缩类，构造函数

Compression::Compression(QWidget *parent) : QMainWindow(parent) {

}
//DEL 函数用于删除Huffman树节点及其子节点。递归
void Compression::DEL(HuffmanTreeNode*& root) {
    if(root == NULL) return;
    DEL(root->leftson);
    DEL(root->rightson);
    delete root;
}
//BinaryString_Init 函数初始化二进制字符串，根据字符映射表将源字符串转换为对应的二进制字符串。
void Compression:: BinaryString_Init() {
    for (int i = 0; i < source_string.size(); i++) {
        binary_string += passwordmap[source_string[i]];
    }
}
//Weightmap_Init 函数根据文件内容初始化字符频率映射表。
void Compression::Weightmap_Init(QFile& in) {


    char*  ch;

    QByteArray ba = fileEnd.toLatin1();

    ch = ba.data();
    source_string.append(ch);

    QByteArray a;
    while (!in.atEnd()) {
        a = in.read(1024);
        string b = a.toStdString();
        for(int i = 0; i < b.size(); i++) {
            unsigned char ch = b[i];
            source_string += ch;

        }
    }
    for(int i = 0; i < source_string.size(); i++) {
        unsigned char ch = source_string[i];
        weightmap[ch]++;
    }

}
//MakehuffmanTreeNode 函数创建一个Huffman树节点。
HuffmanTreeNode* Compression::MakehuffmanTreeNode(int i) {
    HuffmanTreeNode* huffman = new HuffmanTreeNode;
    huffman->Weight.first = i;
    huffman->Weight.second = weightmap[i];
    huffman->tag = 0;
    huffman->leftson = NULL;
    huffman->rightson = NULL;
    return huffman;
}
//HuffmanTreeVector_Init 函数初始化Huffman树节点的容器。
void Compression::HuffmanTreeVector_Init() {
    for (map<unsigned char, int>::iterator it = weightmap.begin(); it != weightmap.end(); it++) {
        HuffmanTreeNode* huffman = MakehuffmanTreeNode(it->first);
        container.push_back(huffman);
    }
}
//HuffmanTree_Init 函数构建Huffman树。
void Compression::HuffmanTree_Init() {
    //循环删除和添加
    while (container.size() != 1) {
        //排序
        sort(container.begin(), container.end(), compare);
        int sum = container[0]->Weight.second + container[1]->Weight.second;
        HuffmanTreeNode* newhuffman = new HuffmanTreeNode;
        newhuffman->Weight.second = sum;
        newhuffman->Weight.first = 0;
        newhuffman->tag = 1;
        newhuffman->leftson = container[0];
        newhuffman->rightson = container[1];
        container.erase(container.begin());
        container.erase(container.begin());
        container.push_back(newhuffman);
    }
}
//ZipPassword_Init 函数初始化Huffman树节点的压缩密码，即每个字符对应的二进制编码。
void Compression::ZipPassword_Init(HuffmanTreeNode* &root, string& password) {
    //为叶子节点创建哈夫曼编码
    if (root != NULL && !root->tag) {
        root->zippassword = password;
        passwordmap[root->Weight.first] = password;
    }
    if (root->leftson != NULL) {
        ZipPassword_Init(root->leftson, password += "0");
        //返回上一级前将添加的字符弹出
        password.pop_back();
    }
    if (root->rightson != NULL) {
        ZipPassword_Init(root->rightson, password += "1");
        //返回上一级前将添加的字符弹出
        password.pop_back();
    }
}
//zip 函数实现文件压缩操作，包括字符频率统计、Huffman树构建、压缩密码生成、二进制字符串初始化等。
void Compression::zip(QString path, QString pathAfter) {


    //记录开始时间
    clock_t begin = clock();
    //打开文件
    QFile openfile(path);
    if(!openfile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(NULL, QString("警告"), QString("文件打开失败"));
        emit error();
        return;
    }
    if(pathAfter == "") {
        QMessageBox::information(NULL, QString("警告"), QString("文件打开失败"));
        emit error();
        return;
    } else {
        QStringList list1 = path.split("/");
        QString tem = list1[list1.size() - 1];
        QStringList temp = tem.split(".");
        QString name = temp[0];
        fileEnd = temp[1];
        qDebug() << fileEnd;
        endNum = fileEnd.size();
        pathAfter += "/";
        pathAfter += name;
        pathAfter += ".HuffmanZip";
    }
    qDebug() << fileEnd;
    //给字符赋予权重
    Weightmap_Init(openfile);

    //调节进度条
    emit mysignal(10);


    //初始化霍夫曼树节点
    HuffmanTreeVector_Init();
    emit mysignal(20);
    //构建霍夫曼树
    HuffmanTree_Init();
    emit mysignal(30);
    string empty = "";
    //获得霍夫曼编码
    ZipPassword_Init(container[0], empty);
    emit mysignal(40);
    //将源字符串转变为二进制字符串
    BinaryString_Init();
    emit mysignal(50);

    openfile.close();
    QFile savefile(pathAfter);
    savefile.open(QIODevice::WriteOnly);
    QDataStream out(&savefile);
    int size = passwordmap.size();
    if (size == 256) size = 0;
    int length = 0;
    //写入密码表的长度
    out << size;
    length++;
    double k = 1;
    for (map<unsigned char, string>::iterator it = passwordmap.begin(); it != passwordmap.end(); it++) {
        emit mysignal(50 + double(25 * k++) / passwordmap.size());
        int first = it->first;
        //写入字符
        out << first;
        length++;
        string second = it->second;
        int size = second.size();
        //写入密码长度
        out << size;
        length++;
        //写入密码
        int n = 8 - second.size() % 8;
        if (n) {
            //不足的补o
            second.append(n, '0');
        }
        for (int i = 0; i < second.size(); i += 8) {

            string k = second.substr(i, 8);
            int temp = binarystringtoint(k);
            unsigned char ch = temp;
            out << ch;
            length++;
        }
    }
    //写入源文件的二进制
    int n = 8 - binary_string.size() % 8;
    //写入补0的数目
    out << n;
    out << endNum;
    if (n) {
        binary_string.append(n, '0');
    }
    length++;
    int totalbitsize = binary_string.size() / 8;
    for (int i = 0; i < binary_string.size(); i += 8) {
        emit mysignal(75 + double(25 * i) / binary_string.size());
        string k = binary_string.substr(i, 8);
        int temp = binarystringtoint(k);
        unsigned char ch = temp;
        out << ch;
        length++;
    }


    emit mysignal(100);
    int newlength = savefile.size();
    savefile.close();
    clock_t end = clock();
//    QString tip("理论压缩比:");
//    tip += QString::number(double(totalbitsize * 100) / source_string.size());
//    tip += "%,实际压缩比：";
//    tip += QString::number(double(newlength * 100) / source_string.size());
//    tip += "%,压缩用时：";
//    tip += QString::number(double(end - begin) / CLOCKS_PER_SEC);
//    tip += "s";
//    QMessageBox::about(this, "压缩说明", tip);
    weightmap.clear();
    passwordmap.clear();
    source_string.clear();
    binary_string.clear();
    DEL(container[0]);
    container.clear();
}
//void Compression::zip(QString path) {


//    //记录开始时间
//    clock_t begin = clock();
//    //打开文件
//    QFile openfile(path);
//    if(!openfile.open(QIODevice::ReadOnly)) {
//        QMessageBox::information(NULL, QString("警告"), QString("文件打开失败"));
//        emit error();
//        return;
//    }

//    qDebug() << fileEnd;
//    //给字符赋予权重
//    Weightmap_Init(openfile);

//    //调节进度条
//    emit mysignal(10);


//    //初始化霍夫曼树节点
//    HuffmanTreeVector_Init();
//    emit mysignal(20);
//    //构建霍夫曼树
//    HuffmanTree_Init();
//    emit mysignal(30);
//    string empty = "";
//    //获得霍夫曼编码
//    ZipPassword_Init(container[0], empty);
//    emit mysignal(40);
//    //将源字符串转变为二进制字符串
//    BinaryString_Init();
//    emit mysignal(50);

//    openfile.close();
//    QFile savefile(pathAfter);
//    savefile.open(QIODevice::WriteOnly);
//    QDataStream out(&savefile);
//    int size = passwordmap.size();
//    if (size == 256) size = 0;
//    int length = 0;
//    //写入密码表的长度
//    out << size;
//    length++;
//    double k = 1;
//    for (map<unsigned char, string>::iterator it = passwordmap.begin(); it != passwordmap.end(); it++) {
//        emit mysignal(50 + double(25 * k++) / passwordmap.size());
//        int first = it->first;
//        //写入字符
//        out << first;
//        length++;
//        string second = it->second;
//        int size = second.size();
//        //写入密码长度
//        out << size;
//        length++;
//        //写入密码
//        int n = 8 - second.size() % 8;
//        if (n) {
//            //不足的补o
//            second.append(n, '0');
//        }
//        for (int i = 0; i < second.size(); i += 8) {

//            string k = second.substr(i, 8);
//            int temp = binarystringtoint(k);
//            unsigned char ch = temp;
//            out << ch;
//            length++;
//        }
//    }
//    //写入源文件的二进制
//    int n = 8 - binary_string.size() % 8;
//    //写入补0的数目
//    out << n;
//    out << endNum;
//    if (n) {
//        binary_string.append(n, '0');
//    }
//    length++;
//    int totalbitsize = binary_string.size() / 8;
//    for (int i = 0; i < binary_string.size(); i += 8) {
//        emit mysignal(75 + double(25 * i) / binary_string.size());
//        string k = binary_string.substr(i, 8);
//        int temp = binarystringtoint(k);
//        unsigned char ch = temp;
//        out << ch;
//        length++;
//    }


//    emit mysignal(100);
//    int newlength = savefile.size();
//    savefile.close();
//    clock_t end = clock();
//    //    QString tip("理论压缩比:");
//    //    tip += QString::number(double(totalbitsize * 100) / source_string.size());
//    //    tip += "%,实际压缩比：";
//    //    tip += QString::number(double(newlength * 100) / source_string.size());
//    //    tip += "%,压缩用时：";
//    //    tip += QString::number(double(end - begin) / CLOCKS_PER_SEC);
//    //    tip += "s";
//    //    QMessageBox::about(this, "压缩说明", tip);
//    weightmap.clear();
//    passwordmap.clear();
//    source_string.clear();
//    binary_string.clear();
//    DEL(container[0]);
//    container.clear();
//}
//unzip 函数实现文件解压缩操作，包括读取压缩文件中的信息，构建压缩密码映射表，解析二进制字符串并写入解压文件。
void Compression::unzip(QString path, QString pathAfter) {
    if(pathAfter == "") {
        QMessageBox::information(NULL, QString("警告"), QString("输出文件位置为空"));
        emit error();
        return;
    }
    //记录开始时间
    clock_t begin = clock();
    //判断后缀名是否符合

    QFile openfile(path);
    if(!openfile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(NULL, QString("警告"), QString("文件打开失败"));
        emit error();
        return;
    }

    QDataStream in(&openfile);
    //读入密码表的长度
    int zipmapsize;
    in >> zipmapsize;
    if (zipmapsize == 0) zipmapsize = 256;
    for (int i = 1; i <= zipmapsize; i++) {
        //对应的字符
        int zipkey;
        in >> zipkey;
        //密码表的长度
        int valuelength;
        in >> valuelength;
        string valuestring;
        emit mysignal(double(10 * i) / zipmapsize);

        for (int j = 1; j <= valuelength / 8 + 1; j++) {
            unsigned char ch;
            in >> ch;
            int zipvalue = ch;
            //将整数转变为二进制字符串
            valuestring += inttobinarystring(zipvalue);
        }
        //消去补位的0
        valuestring.erase(valuelength, valuestring.size() - valuelength );
        zippassword[valuestring] = zipkey;
    }
    string zipstring;
    //读入0的数目
    int num;
    in >> num;
    //读入后缀名数
    int postNum;
    in >> postNum;
//    qDebug() << postNum;
//    qDebug() << zippassword.size();

    emit mysignal(30);
    //读取压缩文件
    while (!in.atEnd()) {
        unsigned char ch;
        in >> ch;
        int bit = ch;
        zipstring += inttobinarystring(bit);
    }
    emit mysignal(50);

    //末位0的数目

    zipstring.erase(zipstring.size() - num, num );
    string endString = "";
    string s = "";
    int j = 0;
    int c = 0;
    while (j < postNum && c < zipstring.size()) {
        s += zipstring[c];
        map<string, int>::iterator it = zippassword.find(s);
        if (it != zippassword.end()) {
            unsigned char temp = it->second;

            endString += temp;
            s = "";
            j++;
        }
        c++; // 在循环内递增 c
    }

    openfile.close();
    pathAfter.append(".");
    pathAfter.append(endString);
    qDebug() << endString;
    qDebug() << pathAfter;
    QFile savefile(pathAfter);
    savefile.open(QIODevice::WriteOnly);
    string str;
    for (int i = c; i < zipstring.size(); i++) {
        emit mysignal(50 + double(50 * i) / zipstring.size());
        str += zipstring[i];
        map<string, int>::iterator it = zippassword.find(str);
        if (it != zippassword.end()) {
            unsigned char temp = it->second;
            //用二进制方式写入
            savefile.write(reinterpret_cast<char*>(&temp), 1);
            str.clear();
        }
    }
    emit mysignal(100);
    savefile.close();
//    clock_t end = clock();
//    QString tip = "解压用时:";
//    tip += QString::number(double(end - begin) / CLOCKS_PER_SEC);
//    tip += "s";
//    QMessageBox::about(this, "解压说明", tip);
    zippassword.clear();
}

void Compression::zipSingleFile(const QString& filePath, QString outputPath) {
    // 这里放置你单个文件的压缩逻辑
    // 例如，打开文件，进行压缩，保存到输出路径等
}
void Compression::zip(const QStringList& filePaths, const QString& folderPath, QString outputPath) {
    QStringList tempPaths; // To store the paths of intermediate compressed files
    QDir folderDir(folderPath);
    QString tempPath;
    for (const QString& filePath : filePaths) {
        QFile inputFile(filePath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open input file: " << filePath;
            continue;
        }

        // Use a temporary file path for each input file
        QStringList list1 = filePath.split("/");
        QString tem = list1[list1.size() - 1];
        QStringList temp = tem.split(".");
        QString name = temp[0];
        fileEnd = temp[1];
        qDebug() << fileEnd;
        endNum = fileEnd.size();


        list1.remove(list1.size() - 1, 1);
        QString tem1;
        for(int i = 0; i < list1.size(); i++) {
            tem1 += list1[i] + "/";
        }
        tem1.removeAt(tem1.size() - 1);
        // Compress the input file and save the compressed file to the temporary path
        zip(filePath, tem1);



        tem1 += "/";
        tem1 += name;
        tem1 += ".HuffmanZip";
        tempPaths.append(tem1);
        qDebug() << "nnnnnnnn" << tem1;
        inputFile.close();
    }
    QStringList tempOut = outputPath.split("/");

    tempPath = outputPath + "/" + tempOut[tempOut.size() - 1] + ".huff";
    qDebug() << "                    " + tempOut[tempOut.size() - 1] ;
    qDebug() << "                    " + tempPath;
    QFile outputZipFile(tempPath);
    if (!outputZipFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open output zip file";
        return;
    }

    QDataStream out(&outputZipFile);

    // Write the number of files in the archive
    out << static_cast<quint64>(filePaths.size());

    // Loop through each file and compress
    for (const QString& filePath : tempPaths) {
        QFile inputFile(filePath);
        if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open input file: " << filePath;
            continue;
        }

        QByteArray fileData = inputFile.readAll();
        inputFile.close();

        // Write the file name length and file name
        out << static_cast<quint64>(folderDir.relativeFilePath(filePath).size());

        qDebug() << ">>>>>>>>>>>>" + folderDir.relativeFilePath(filePath);
        out.writeRawData(folderDir.relativeFilePath(filePath).toUtf8(), folderDir.relativeFilePath(filePath).size());

        // Write the file content length and content
        out << static_cast<quint64>(fileData.size());
        out.writeRawData(fileData.data(), fileData.size());
        QFile::remove(filePath);
    }


    outputZipFile.close();
}

void Compression::zipFolder(const QString& folderPath, const QString& outputZipPath) {
    QDir folderDir(folderPath);

    if (!folderDir.exists()) {
        QMessageBox::information(NULL, QString("警告"), QString("文件夹不存在"));
        emit error();
        return;
    }

    QStringList fileNames;
    QStringList relativeFileNames; // Added to store relative paths

    // Recursively get all files within the folder and its subfolders
    getAllFilesInFolder(folderPath, fileNames, relativeFileNames);

    // 将文件列表压缩为一个压缩文件
    zip(fileNames, folderPath, outputZipPath); // Pass both fileNames and relativeFileNames
}

void Compression::getAllFilesInFolder(const QString& folderPath, QStringList& fileList, QStringList& relativeFileList) {
    QDir folderDir(folderPath);

    QFileInfoList fileInfoList = folderDir.entryInfoList(QDir::Files);
    for (const QFileInfo& fileInfo : fileInfoList) {
        fileList.append(fileInfo.absoluteFilePath());
        relativeFileList.append(folderDir.relativeFilePath(fileInfo.absoluteFilePath())); // Record relative path
    }

    // Recursively get files from subfolders
    QStringList subfolderList = folderDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& subfolder : subfolderList) {
        QString subfolderPath = folderPath + "/" + subfolder;
        getAllFilesInFolder(subfolderPath, fileList, relativeFileList);
    }
}


void Compression::unzipDir(const QString& inputPath, const QString& outputDir) {
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open input zip file: " << inputPath;
        return;
    }

    QDataStream in(&inputFile);

    // Read the number of files in the archive
    quint64 numFiles;
    in >> numFiles;

    // Loop through each file and decompress
    for (quint64 i = 0; i < numFiles; ++i) {
        // Read the file name length and file name
        quint64 fileNameLength;
        in >> fileNameLength;
        QByteArray fileNameData(fileNameLength, '\0');
        in.readRawData(fileNameData.data(), fileNameLength);
        QString fileName = QString::fromUtf8(fileNameData);

        // Read the file content length and content
        quint64 fileContentLength;
        in >> fileContentLength;
        QByteArray fileContent(fileContentLength, '\0');
        in.readRawData(fileContent.data(), fileContentLength);

        // Construct the relative output file path
        QString relativeFilePath = fileName;
        QString name = fileName.split(".")[0];

        qDebug() << ">>>>>>>>>>>>>>>>" + relativeFilePath;

        // Construct the output file path with relative structure
        QString outputFilePath = outputDir + "/" + relativeFilePath;

        // Create directories if they don't exist
        QDir().mkpath(QFileInfo(outputFilePath).path());

        // Write the decompressed content to the output file
        QFile outputFile(outputFilePath);
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open output file: " << outputFilePath;
            continue;
        }
        outputFile.write(fileContent);
        outputFile.close();
        // 解压缩单个文件
        unzip(outputFilePath, outputDir + "/" + name);

        // 删除中间产生的压缩文件
        QFile::remove(outputFilePath);
    }

    inputFile.close();
}

// 小学生测验.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include <cstdlib>
#include <list>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

int Number;

void Update_Number() {
    ifstream config("config");
    config >> Number;
    if (config.fail()) {
        ofstream config("config");
        Number = 1;
        config << Number;
        config.close();
    }
    
}

class Question {
public:
    int num1;
    int num2;
    char operatorSymbol;
    int correctAnswer;
    int userAnswer;
    bool isAnswered;

    // 构造函数
    Question() : num1(0), num2(0), operatorSymbol('+'), correctAnswer(0), userAnswer(0), isAnswered(false) {}

    // 从文件读取试题到试题列表的函数
    bool readQuestionsFromFile(const string& filename, list<Question>& testQuestions) {
        testQuestions.clear();
        ifstream inFile(filename);
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                Question question;
                // 创建字符串流用于解析每行数据
                stringstream ss(line);
                string questionNumberStr;
                char dot;
                ss >> questionNumberStr >> question.num1 >> question.operatorSymbol >> question.num2 >> dot >> question.correctAnswer;
                // 将解析出的数据存入Question结构体实例
                question.isAnswered = false;
                question.userAnswer = 0;
                testQuestions.push_back(question);
            }
            inFile.close();
            return true;
        }
        else {
            cerr << "无法打开文件读取试题，请检查文件名及文件权限等问题。" << endl;
            return false;
        }
    }

};

class UserNode {
private:
    std::string username;
    std::string password;
    UserNode* next;

public:
    // 构造函数
    UserNode(std::string u, std::string p) : username(std::move(u)), password(std::move(p)), next(nullptr) {}

    // 复制构造函数
    UserNode(const UserNode& other) : username(other.username), password(other.password), next(nullptr) {
        if (other.next != nullptr) {
            // 如果原节点的next指针不为空，递归地复制后续节点链表
            next = new UserNode(*(other.next));
        }
    }

    // 获取用户名的函数
    std::string getUsername() const {
        return username;
    }

    // 获取密码的函数
    std::string getPassword() const {
        return password;
    }

    // 获取下一个节点指针的函数
    UserNode* getNext() const {
        return next;
    }

    // 设置下一个节点指针的函数
    void setNext(UserNode* n) {
        next = n;
    }
};

class Student{
public:
    string Username{};
    string Password{};
    int score;
    list<Question> testQuestions;
    time_t startTime;
    time_t endTime;
public:
    //空白构造函数;
    Student(){  
        score=0;
        startTime=0;
        endTime=0;
    }

    // 重置学生信息
    void reset() {
        Username.clear();
        Password.clear();
        score = 0;
        startTime = 0;
        endTime = 0;
    }

    // 重置试题信息
    void reset_Q() {
        score = 0;
        testQuestions.clear();
    }

    // 获取测试开始时间
    time_t getStartTime() const {
        return startTime;
    }

    // 获取测试结束时间
    time_t getEndTime() const {
        return endTime;
    }

    string getUsername() {
        return Username;
    }
    string getPassword() {
        return Password;
    }

    // 保存测试记录
    void saveTestRecord() {
        ofstream outFile(Username + ".txt", ios_base::app);
        if (outFile.is_open()) {
            outFile << "用户名: " << Username << endl;
            outFile << "得分: " << score << endl;
            char startTimeStr[26];
            char endTimeStr[26];
            ctime_s(startTimeStr, sizeof(startTimeStr), &startTime);
            ctime_s(endTimeStr, sizeof(endTimeStr), &endTime);
            outFile << "开始时间: " << startTimeStr;
            outFile << "结束时间: " << endTimeStr;

            outFile << "试题信息:" << endl;
            int questionNumber = 1;
            for (const auto& question : testQuestions) {
                outFile << questionNumber << ". " << question.num1 << " " << question.operatorSymbol << " "
                    << question.num2 << " = ";
                if (question.isAnswered) {
                    outFile << question.userAnswer << " (正确)" << endl;
                }
                else {
                    outFile << "未回答" << endl;
                }
                questionNumber++;
            }
            outFile << "*******************************" << endl;

            outFile.close();
        }
        else {
            cerr << "无法打开文件保存测试记录。" << endl;
        }
    }
};

// 从文件中读取用户名和密码到链表，参数改为类类型的指针引用
void readFromFile(const string& filename, UserNode*& head) {
    head = nullptr;
    UserNode* tail = nullptr;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string username, password;
            if (ss >> username >> password) {
                auto* newNode = new UserNode(username, password);
                if (head == nullptr) {
                    head = newNode;
                    tail = newNode;
                }
                else {
                    tail->setNext(newNode);
                    tail = newNode;
                }
            }
        }
        file.close();
    }
}

// 比较用户输入的用户名和密码与链表中存储的数据
int compareUser(const UserNode* head, const string& inputUsername, const string& inputPassword) {
    const UserNode* current = head;
    while (current != nullptr) {
        if (current->getUsername() == inputUsername) {
            // 直接获取链表节点中存储的密码
            string storedPassword = current->getPassword();
            if (inputPassword == storedPassword) {
                return 1;
            }
            else {
                return 2;  // 用户已注册但是密码错误
            }
        }
        current = current->getNext();
    }
    return 0;
}

// 将新的用户名和密码写入文件
void writeToFile(const string& filename, const string& username, const string& password) {
    ofstream file(filename, ios::app);  // 以追加模式打开文件
    if (file.is_open()) {
        file << username << " " << password << endl;
        file.close();
        cout << "注册成功，信息已保存！" << endl;
    }
    else {
        cerr << "无法打开文件进行写入操作！" << endl;
    }
}

// 释放链表内存，参数改为类类型的指针
void freeList(UserNode* head) {
    UserNode* current = head;
    while (current != nullptr) {
        UserNode* next = current->getNext();
        delete current;
        current = next;
    }
}

// 先输入用户名、密码登录，以方便保存学生信息。
int login(Student& student) {
    UserNode* userList = nullptr;
    readFromFile("user", userList);
    cout << "请输入您的用户名(未注册用户将自动注册):";
    cin >> student.Username;
    cout << "请输入您的密码:";
    cin >> student.Password;
    // 比较用户输入和链表中的数据
    if (compareUser(userList, student.Username, student.Password) == 1) {
        cout << "登录成功！" << endl;
        freeList(userList);
        cout << "正在进入小学生测试" << endl;
        this_thread::sleep_for(std::chrono::milliseconds(500));
        return 1;
    }
    else if (compareUser(userList, student.Username, student.Password) == 0) {
        writeToFile("user", student.Username, student.Password);
        freeList(userList);
        cout << "正在进入小学生测试" << endl;
        this_thread::sleep_for(std::chrono::milliseconds(500));
        return 1;
    }
    else {
        cout << "密码错误" << endl;
        freeList(userList);
        return 0;
    }

}

void GUI_main(Student& student) {
    cout << "*************************" << endl;
    cout << "欢迎" << student.Username << "来到一二年级小学生测验!!!" << endl;
    cout << "1.开始答题" << endl;
    cout << "2.查看试卷信息" << endl;
    cout << "3.查看排行榜" << endl;
    cout << "4.切换账号" << endl;
    cout << "5.查看历史记录" << endl;
    cout << "6.退出程序" << endl;
    cout << "*************************" << endl;
}




// 生成随机试题函数
void generateQuestions(list<Question>& testQuestions) {
    std::mt19937 generator;  // 创建一个默认初始化的 Mersenne Twister 随机数生成引擎，不过这样每次运行结果一样，通常需要设置种子
    std::random_device rd;  // 用于获取真正随机的种子值（依赖硬件熵源等）
    generator.seed(rd());  // 使用随机设备获取的随机值作为种子初始化引擎

    for (int i = 0; i < 10; ++i) {
        Question question{};
        question.num1 = generator() % 50;
        question.num2 = generator() % 50;
        int operationChoice = generator() % 2;
        if (operationChoice == 0) {
            question.operatorSymbol = '+';
            question.correctAnswer = question.num1 + question.num2;
            if (question.correctAnswer > 50) {
                // 确保结果不大于50，重新生成加法题目
                i--;
                continue;
            }
        }
        else {
            question.operatorSymbol = '-';
            question.correctAnswer = question.num1 - question.num2;
            if (question.correctAnswer < 0) {
                // 确保结果非负，重新生成减法题目
                i--;
                continue;
            }
        }
        question.isAnswered = false;
        testQuestions.push_back(question);
    }
}

// 显示试题函数
void Questions_show(Student& student) {
    cout << "这是尚未被保存的第" << Number << "套试题" << endl;
    int questionNumber = 1;
    for (const Question& question : student.testQuestions) {
        cout << questionNumber << ". " << question.num1 << " " << question.operatorSymbol << " "
            << question.num2 << " = " << question.correctAnswer << endl;
        questionNumber++;
    }
}

// 保存试题到本地函数
void Question_save(Student& student) {
    std::stringstream ss;
    ss << Number << ".txt";
    std::string fileName = ss.str();
    ofstream outFile(fileName);
    if (outFile.is_open()) {
        int questionNumber = 1;
        for (const Question& question : student.testQuestions) {
            outFile << questionNumber << ". " << question.num1 << " " << question.operatorSymbol << " "
                << question.num2 << " = " << question.correctAnswer << endl;
            questionNumber++;
        }
        Number++;
        ofstream config("config");
        config << Number;
        config.close();
        cout << "保存成功!" << endl;
        this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// 答题函数
void takeTest(Student& student) {
    student.startTime = time(nullptr);
    int questionNumber = 1;
    for (Question& question : student.testQuestions) {
        int tries = 0;
        while (tries < 3) {
            cout << questionNumber << ". " << question.num1 << " " << question.operatorSymbol << " "
                << question.num2 << " = ";
            cin >> question.userAnswer;

            if (question.userAnswer == question.correctAnswer) {
                if (tries == 0) {
                    student.score += 10;
                }
                else if (tries == 1) {
                    student.score += 7;
                }
                else {
                    student.score += 5;
                }
                question.isAnswered = true;
                break;
            }
            else {
                cout << "回答错误，请重新输入。" << endl;
                tries++;
            }
        }
        if (tries == 3) {
            cout << "正确答案是: " << question.correctAnswer << endl;
        }
        questionNumber++;
    }
    student.endTime = time(nullptr);
}

// 修改试题函数
void modifyQuestion(list<Question>& testQuestions) {
    int questionNumber;
    cout << "请输入要修改的试题编号(1 - 10): ";
    cin >> questionNumber;

    if (questionNumber >= 1 && questionNumber <= 10) {
        auto it = testQuestions.begin();
        advance(it, questionNumber - 1);

        cout << "请输入新的第一个数: ";
        cin >> it->num1;
        cout << "请输入新的第二个数: ";
        cin >> it->num2;

        int operationChoice;
        cout << "请选择运算(0: 加法, 1: 减法): ";
        cin >> operationChoice;

        if (operationChoice == 0) {
            it->operatorSymbol = '+';
            it->correctAnswer = it->num1 + it->num2;
        }
        else {
            it->operatorSymbol = '-';
            it->correctAnswer = it->num1 - it->num2;
            if (it->correctAnswer < 0) {
                cout << "减法结果不能为负数，请重新输入。" << endl;
                modifyQuestion(testQuestions);
                return;
            }
        }

        it->isAnswered = false;
    }
    else {
        cout << "无效的试题编号。" << endl;
    }
}

// 插入试题函数
void insertQuestion(list<Question>& testQuestions) {
    Question question{};
    cout << "请输入要插入的第一个数: ";
    cin >> question.num1;
    cout << "请输入要插入的第二个数: ";
    cin >> question.num2;

    int operationChoice;
    cout << "请选择运算(0: 加法, 1: 减法): ";
    cin >> operationChoice;

    if (operationChoice == 0) {
        question.operatorSymbol = '+';
        question.correctAnswer = question.num1 + question.num2;
    }
    else {
        question.operatorSymbol = '-';
        question.correctAnswer = question.num1 - question.num2;
        if (question.correctAnswer < 0) {
            cout << "减法结果不能为负数，请重新输入。" << endl;
            insertQuestion(testQuestions);
            return;
        }
    }

    question.isAnswered = false;

    int insertPosition;
    cout << "请输入要插入的位置(1 - " << testQuestions.size() + 1 << "): ";
    cin >> insertPosition;

    if (insertPosition >= 1 && insertPosition <= testQuestions.size() + 1) {
        auto it = testQuestions.begin();
        advance(it, insertPosition - 1);
        testQuestions.insert(it, question);
    }
    else {
        cout << "无效的插入位置。" << endl;
    }
}

// 删除试题函数
void deleteQuestion(list<Question>& testQuestions) {
    int questionNumber;
    cout << "请输入要删除的试题编号(1 - " << testQuestions.size() << "): ";
    cin >> questionNumber;

    if (questionNumber >= 1 && questionNumber <= testQuestions.size()) {
        auto it = testQuestions.begin();
        advance(it, questionNumber - 1);
        testQuestions.erase(it);
    }
    else {
        cout << "无效的试题编号。" << endl;
    }
}

// 显示结果函数
void displayResult(Student& student) {
    cout << "你的得分是: " << student.score << endl;
    if (student.score >= 90) {
        cout << "SMART" << endl;
    }
    else if (student.score >= 80 && student.score < 90) {
        cout << "GOOD" << endl;
    }
    else if (student.score >= 70 && student.score < 80) {
        cout << "OK" << endl;
    }
    else if (student.score >= 60 && student.score < 70) {
        cout << "PASS" << endl;
    }
    else {
        cout << "TRY AGAIN" << endl;
    }
}

// 比较函数，用于按照成绩进行排序（从大到小排序）
bool compareStudents(const Student& s1, const Student& s2) {
    return s1.score > s2.score;
}

// 向排行榜文件追加写入学生信息的函数
void writeToLeaderboard(const Student& student) {
    std::ofstream outFile("Leaderboard.txt", std::ios::app);  // 以追加模式打开文件
    if (outFile.is_open()) {
        outFile << student.Username << " " << student.score << std::endl;
        outFile.close();
    }
    else {
        std::cerr << "无法打开排行榜文件进行写入，请检查文件权限或是否存在。" << std::endl;
    }
}

// 排行榜排序
void Leaderboard(const Student& student) {
    vector<Student> students;
    ifstream inputFile("Leaderboard.txt");
    if (inputFile) {
        string line;
        while (getline(inputFile, line)) {
            Student student;
            istringstream iss(line);
            if (iss >> student.Username >> student.score) {
                students.push_back(student);
            }
        }
        inputFile.close();

        // 使用标准库的排序算法进行排序
        sort(students.begin(), students.end(), compareStudents);
    }
    else {
        cerr << "无法打开文件 Leaderboard.txt" << endl;
    }

    // 输出排序后的结果
    cout << "********排行榜***********" << endl;
    int rank = 1;
    for (const auto& s : students) {
        cout << rank << "." << s.Username << " " << s.score << endl;
        rank++;
    }
}

void question_GUI() {
    cout << "*************************" << endl;
    cout << "欢迎来到试题编辑页面" << endl;
    cout << "1.修改试题" << endl;
    cout << "2.新增试题" << endl;
    cout << "3.删除试题" << endl;
    cout << "4.保存当前试题" << endl;
    cout << "5.重置试题" << endl;
    cout << "6.读取试题" << endl;
    cout << "7.返回上一级" << endl;
    cout << "*************************" << endl;
}

void his_GUI() {
    cout << "*************************" << endl;
    cout << "1.返回上一级" << endl;
    cout << "*************************" << endl;
}
void ans_GUI() {
    cout << "*************************" << endl;
    cout << "1.返回上一级" << endl;
    cout << "*************************" << endl;
}

int main()
{
    Update_Number();
    Student student;
    bool login_state = false;
    while (login_state == 0) {
        int result = login(student);
        if (result == 1) {
            login_state = true;
            break;
        }
    }
    

    // 生成试题
    generateQuestions(student.testQuestions);
    while (login_state == 1) {
        system("cls");
        GUI_main(student);
        int in;
        cout << "输入编号以开始:";
        cin >> in;
        if (cin.fail()) {
            // 输入出现错误，清除错误标志位
            cin.clear();
            // 忽略缓冲区中剩余的字符，直到遇到换行符（可以根据实际情况调整参数）
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "输入了非法字符，请重新输入数字！" << endl;
            this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
        switch (in) {
            //开始答题
        case 1: {
            bool answer_state = true;
            system("cls");
            // 进行测试
            takeTest(student);
            // 保存测试记录
            student.saveTestRecord();
            // 显示结果
            displayResult(student);
            writeToLeaderboard(student);
            Leaderboard(student);
            while (answer_state == 1) {
                ans_GUI();
                int ans_in;
                cout << "输入编号以开始:";
                cin >> ans_in;
                if (ans_in == 1) {
                    answer_state = 0;
                }
            }
            break;
        }

              //查看试卷信息
        case 2: {
            system("cls");
            bool state = true;
            while (state) {
                Questions_show(student);
                question_GUI();
                int q_in;
                cout << "输入编号以开始:";
                cin >> q_in;
                switch (q_in) {
                case 1:
                    modifyQuestion(student.testQuestions);
                    break;
                case 2:
                    insertQuestion(student.testQuestions);
                case 3:
                    deleteQuestion(student.testQuestions);
                    break;
                case 4: //保存当前试题
                    Question_save(student);
                    break;
                case 5:
                    student.reset_Q();
                    generateQuestions(student.testQuestions);
                    break;
                case 6:  //从文件中读取试题
                    {
                    cout << "请输入要读取的试题编号:";
                    int num;
                    cin >> num;
                    stringstream ss;
                    ss << num << ".txt";
                    string FileName = ss.str();
                    // 调用Question类的readQuestionsFromFile函数读取试题，注意这里直接传入试题列表
                    if (Question().readQuestionsFromFile(FileName, student.testQuestions)) {
                        cout << "试题读取成功！" << endl;
                    }
                    else {
                        cout << "试题读取失败，请检查相关问题。" << endl;
                    }
                    break;
                    }
                case 7:
                    state = false;
                    break;
                }
            }
        

            break;
        }

              //退出程序
        case 6: {
            system("cls");
            cout << "感谢您的使用,我们下次再会!" << endl;
            cout << "程序将于1s后退出" << endl;
            this_thread::sleep_for(std::chrono::milliseconds(1000));
            exit(1);
        }

              //切换账号
        case 4: {
            system("cls");
            // 调用学生对象的reset函数清除当前学生信息
            student.reset();
            bool login_state_new = false;
            while (login_state_new == false) {
                int result = login(student);
                if (result == 1) {
                    login_state_new = true;
                    // 更新外层的login_state变量，确保后续流程正常
                    login_state = true;
                    break;
                }


            }
            // 如果新的登录失败，将login_state设置为false，避免后续异常流程
            if (!login_state_new) {
                login_state = false;
            }
            break;
        }

              //查看排行榜
        case 3: {
            system("cls");
            bool l_state = true;
            while (l_state) {
                Leaderboard(student);
                cout << "*************************" << endl;
                cout << "1.清除排行榜" << endl;
                cout << "2.返回上一级" << endl;
                cout << "*************************" << endl;
                int l_in;
                cin >> l_in;
                if (l_in == 1) {
                    std::string filename = "Leaderboard.txt";
                    if (std::remove(filename.c_str()) == 0) {
                        std::ofstream file(filename);
                        if (file.is_open()) {
                            file.close();
                            std::cout << "文件内容已成功删除。" << std::endl;
                        }
                        else {
                            std::cerr << "无法创建新文件。" << std::endl;
                        }
                    }
                    else {
                        std::cerr << "无法删除原文件。" << std::endl;
                        break;
                    }
                }
                else if (l_in == 2) {
                    l_state = false;
                    break;
                }
            }
            break;
        }

              //查看历史记录
        case 5: {
            system("cls");
            bool his_state = true;
            while (his_state) {
                std::ifstream inputFile(student.Username + ".txt");
                if (inputFile.is_open()) {
                    std::string line;
                    while (std::getline(inputFile, line)) {
                        std::cout << line << std::endl;
                    }
                    inputFile.close();
                }
                else {
                    std::cerr << "无法打开文件。" << std::endl;
                }
                his_GUI();
                while (his_state == 1) {
                    cout << "输入编号以开始:";
                    int his_in;
                    cin >> his_in;
                    if (his_in == 1) {
                        his_state = false;
                    }
                    else {
                        cout << "请输入正确的数字";
                    }
                }

            }
        }
        }


    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

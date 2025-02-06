#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <map>
#include <fstream>
#include <regex>
#include <windows.h>

using namespace std;
namespace fs = filesystem;

class CLI {
private:
    string prompt = "CLI> ";
    vector<string> command_history;
    map<string, string> aliases;

    void list_directory(const string& path) {
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                cout << (entry.is_directory() ? "[DIR] " : "      ") << entry.path().filename().string() << "\n";
            }
        } catch (const exception& e) {
            set_color(4);
            cerr << "Error listing directory: " << e.what() << "\n";
            log("Error listing directory: " + string(e.what()));
            reset_color();
        }
    }

    void change_directory(const string& path) {
        try {
            fs::current_path(path);
        } catch (const exception& e) {
            set_color(4); 
            cerr << "Error changing directory: " << e.what() << "\n";
            log("Error changing directory: " + string(e.what()));
            reset_color();
        }
    }

    void display_file(const string& filename) {
        ifstream file(filename);
        if (!file) {
            set_color(4);
            cerr << "File not found or unable to open: " << filename << "\n";
            log("File not found or unable to open: " + filename);
            reset_color();
            return;
        }
        string line;
        while (getline(file, line)) {
            cout << line << "\n";
        }
    }

    void copy_file(const std::string& src, const std::string& dest) {
        try {
            fs::path source_path = src;
            fs::path dest_path = dest;
            
            if (fs::is_directory(dest_path)) {
                dest_path /= source_path.filename();
            }
            
            fs::copy(source_path, dest_path, fs::copy_options::overwrite_existing);
            set_color(10);
            std::cout << "File copied successfully.\n";
            reset_color();
        } catch (const std::exception& e) {
            set_color(4);
            std::cerr << "Error copying file: " << e.what() << "\n";
            reset_color();
        }
    }

    void move_file(const string& src, const string& dest_folder) {
        try {
            if (!fs::exists(dest_folder) || !fs::is_directory(dest_folder)) {
                set_color(4);
                cerr << "Destination folder does not exist: " << dest_folder << "\n";
                reset_color();
                return;
            }

            string filename = fs::path(src).filename().string();
            fs::path dest = fs::path(dest_folder) / filename;

            fs::rename(src, dest);
            set_color(10);
            cout << "File moved successfully to folder: " << dest_folder << "\n";
            reset_color();
        } catch (const exception& e) {
            set_color(4); 
            cerr << "Error moving file: " << e.what() << "\n";
            log("Error moving file: " + string(e.what()));
            reset_color();
        }
    }

    void remove_file(const string& filename) {
        try {
            if (fs::is_directory(filename)) {
                fs::remove_all(filename);
                set_color(10);
                cout << "Directory and contents deleted successfully.\n";
            } else if (fs::remove(filename)) {
                set_color(10);
                cout << "File deleted successfully.\n";
            } else {
                set_color(4);
                cerr << "File not found or unable to delete.\n";
            }
            reset_color();
        } catch (const exception& e) {
            set_color(4);
            cerr << "Error deleting: " << e.what() << "\n";
            log("Error deleting: " + string(e.what()));
            reset_color();
        }
    }

    void rename_file(const string& old_name, const string& new_name) {
        try {
            fs::rename(old_name, new_name);
            set_color(10); 
            cout << "File renamed successfully.\n";
            reset_color();
        } catch (const exception& e) {
            set_color(4); 
            cerr << "Error renaming file: " << e.what() << "\n";
            log("Error renaming file: " + string(e.what()));
            reset_color();
        }
    }

    void create_folder(const string& folder_name) {
        try {
            if (fs::create_directory(folder_name)) {
                set_color(10); 
                cout << "Folder created successfully.\n";
                reset_color();
            } else {
                set_color(4);
                cerr << "Folder already exists or unable to create.\n";
                reset_color();
            }
        } catch (const exception& e) {
            cerr << "Error creating folder: " << e.what() << "\n";
            log("Error creating folder: " + string(e.what()));
        }
    }

    void create_file(const string& filename) {
        ofstream file(filename);
        if (file) {
            set_color(10); 
            cout << "File created successfully.\n";
            reset_color();
        } else {
            set_color(4); 
            cerr << "Error creating file: " << filename << "\n";
            log("Error creating file: " + filename);
            reset_color();
        }
    }

    void add_alias(const string& alias, const string& command) {
        aliases[alias] = command;
        set_color(10);
        cout << "Alias added: " << alias << " -> " << command << "\n";
        reset_color();
    }

    void remove_alias(const string& alias) {
        if (aliases.erase(alias)) {
            set_color(10);
            cout << "Alias removed: " << alias << "\n";
            reset_color();
        } else {
            set_color(4);
            cerr << "Alias not found: " << alias << "\n";
            reset_color();
        }
    }

    void clear_screen() {
        system("cls");
    }

    void show_system_info() {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        cout << "Processor Architecture: "
            << (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64" : "x86") << "\n";
        cout << "Number of Processors: " << sysInfo.dwNumberOfProcessors << "\n";

        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memInfo)) {
            cout << "Total RAM: " << static_cast<double>(memInfo.ullTotalPhys) / (1024 * 1024 * 1024) << " GB\n";
            cout << "Available RAM: " << static_cast<double>(memInfo.ullAvailPhys) / (1024 * 1024 * 1024) << " GB\n";
        }

        ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;
        if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalBytes, &freeBytes)) {
            cout << "Total Storage: " << totalBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";
            cout << "Free Storage: " << freeBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";
        }

        cout << "Graphics Cards:\n";
        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);
        int deviceIndex = 0;
        int firstGraphicsCard = -1;
        int secondGraphicsCard = -1;
        
        while (EnumDisplayDevices(NULL, deviceIndex, &dd, 0)) {
            if (firstGraphicsCard == -1) {
                firstGraphicsCard = deviceIndex;
            }
            secondGraphicsCard = deviceIndex;
            deviceIndex++;
        }

        if (firstGraphicsCard != -1) {
            EnumDisplayDevices(NULL, firstGraphicsCard, &dd, 0);
            cout << "  1. " << dd.DeviceString << "\n";
        }

        if (secondGraphicsCard != -1 && secondGraphicsCard != firstGraphicsCard) {
            EnumDisplayDevices(NULL, secondGraphicsCard, &dd, 0);
            cout << "  2. " << dd.DeviceString << "\n";
        }

        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm)) {
            cout << "Display Resolution: " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << "\n";
            cout << "Refresh Rate: " << dm.dmDisplayFrequency << " Hz\n";
        }
    }

    void display_command_history() {
        for (size_t i = 0; i < command_history.size(); ++i) {
            cout << i + 1 << ". " << command_history[i] << "\n";
        }
    }

    void help_menu() {
        cout << "Available commands:\n";
        cout << "  dir                - List files in the current directory\n";
        cout << "  cd <path>          - Change current directory\n";
        cout << "  type <filename>    - Display contents of a file\n";
        cout << "  copy <src> <dest>  - Copy a file\n";
        cout << "  move <src> <dest>  - Move a file from source to destination\n";
        cout << "  del <filename>     - Delete a file\n";
        cout << "  rename <old> <new> - Rename a file\n";
        cout << "  mkdir <foldername> - Create a new folder\n";
        cout << "  create <filename>  - Create an empty file\n";
        cout << "  alias <a> <cmd>    - Create a command alias\n";
        cout << "  unalias <a>        - Remove a command alias\n";
        cout << "  clear              - Clear the screen\n";
        cout << "  sysinfo            - Display system information\n";
        cout << "  history            - Show command history\n";
        cout << "  help               - Show this help menu\n";
        cout << "  exit               - Exit the CLI\n";
    }

    void set_color(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    void reset_color() {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // Default color
    }

    void log(const string& message) {
        ofstream logfile("cli_log.txt", ios::app);
        if (logfile) {
            logfile << message << "\n";
        }
    }

    void parse_and_execute(const string& input) {
        istringstream iss(input);
        string command;
        iss >> command;

        if (aliases.find(command) != aliases.end()) {
            command = aliases[command];
        }

        if (command == "dir") {
            string path;
            iss >> path;
            if (path.empty()) path = fs::current_path().string();
            list_directory(path);
        } else if (command == "cd") {
            string path;
            iss >> path;
            change_directory(path);
        } else if (command == "type") {
            string filename;
            iss >> filename;
            display_file(filename);
        } else if (command == "copy") {
            string src, dest;
            iss >> src >> dest;
            copy_file(src, dest);
        } else if (command == "move") {
            string src, dest_folder;
            iss >> src >> dest_folder;
            move_file(src, dest_folder);
        } else if (command == "del") {
            string filename;
            iss >> filename;
            remove_file(filename);
        } else if (command == "rename") {
            string old_name, new_name;
            iss >> old_name >> new_name;
            rename_file(old_name, new_name);
        } else if (command == "mkdir") {
            string folder_name;
            iss >> folder_name;
            create_folder(folder_name);
        } else if (command == "create") {
            string filename;
            iss >> filename;
            create_file(filename);
        } else if (command == "alias") {
            string alias, cmd;
            iss >> alias >> cmd;
            if (!alias.empty() && !cmd.empty()) {
                add_alias(alias, cmd);
            } else {
                cerr << "Usage: alias <alias_name> <command>\n";
            }
        } else if (command == "unalias") {
            string alias;
            iss >> alias;
            if (!alias.empty()) {
                remove_alias(alias);
            } else {
                cerr << "Usage: unalias <alias_name>\n";
            }
        } else if (command == "clear") {
            clear_screen();
        } else if (command == "sysinfo") {
            show_system_info();
        } else if (command == "history") {
            display_command_history();
        } else if (command == "help") {
            help_menu();
        } else if (command == "exit") {
            exit(0);
        } else {
            cerr << "Unknown command: " << command << "\n";
        }

        command_history.push_back(input);
    }

public:
    void run() {
        string input;
        cout << "============ Welcome to the Custom CLI ================ \n Type 'help' for a list of commands.\n";
        while (true) {
            cout << prompt;
            getline(cin, input);
            if (!input.empty()) {
                parse_and_execute(input);
            }
        }
    }
};

int main() {
    CLI cli;
    cli.run();
    return 0;
}

#include <console.hpp>
#include <system.hpp>
#include <vector.hpp>
#include <file.hpp>

struct Command {
    const char* name;

    using Function = void(const Vector<String>& parts);
    Function* func;
};

void help(const Vector<String>& parts);
void list(const Vector<String>& parts);
void cd(const Vector<String>& parts);
void view(const Vector<String>& parts);
void type(const Vector<String>& parts);
void kill(const Vector<String>& parts);
void clear(const Vector<String>& parts);
void date(const Vector<String>& parts);
void sleep(const Vector<String>& parts);
void tasks(const Vector<String>& parts);
void mem(const Vector<String>& parts);
void snake(const Vector<String>& parts);
void shell(const Vector<String>& parts);
void exit(const Vector<String>& parts);

Command commands[] = {
    { "help", help },
    { "list", list },
    { "cd", cd },
    { "view", view },
    { "type", type },
    { "kill", kill },
    { "clear", clear },
    { "date", date },
    { "sleep", sleep },
    { "tasks", tasks },
    { "mem", mem },
    { "snake", snake },
    { "shell", shell },
    { "exit", exit },
};

void main() {
    System::clear();
    Console::print("                            Welcome to Bug OS!\n", Console::Color::CYAN);
    Console::print("              * You can switch shells by pressing F1-F12 *\n\n", Console::Color::GREEN);

    const char* logo =
        "   \\\n"
        "    '-.__.-'\n"
        "    /oo |--.--,--,--.\n"
        "    \\_.-'._i__i__i_.'\n"
        "          \"\"\"\"\"\"\"\"\"  ";

    Console::printf(Console::Color::RED, "%s\n", logo);

    Console::printf("Bug shell started (terminal %d, pid %d) \n", System::tty(), System::pid());
    Console::printf("Please enter your name: ");
    auto name = Console::getLine();
    System::swd("/"_s + name);

    Console::printf("\nHello, %s!\n", name);
    Console::printf("Type 'help' to view list of commands\n", name);

    while (true) {
        Console::print("[");
        Console::printf(Console::Color::GREEN, "%s", System::pwd());
        Console::printf("]: ");
        auto input = Console::getLine();
        auto parts = input.split();

        if (parts.size() == 0)
            continue;

        bool found = false;
        for (Command command : commands) {
            if (command.name == parts[0]) {
                command.func(parts);
                found = true;
                break;
            }
        }

        if (!found) {
            Console::printf("Command '%s' not found!\n", parts[0]);
        }
    }
}

void help(const Vector<String>&) {
    Console::print("help      - view help\n");
    Console::print("list      - list files in current directory\n");
    Console::print("cd        - change current directory\n");
    Console::print("view file - view content of file\n");
    Console::print("type file - type to file\n");
    Console::print("tasks     - list all tasks running\n");
    Console::print("kill pid  - kill task with specified pid\n");
    Console::print("clear     - clear console\n");
    Console::print("date      - view current date\n");
    Console::print("sleep ms  - sleep for 'ms' millisecs\n");
    Console::print("mem       - view physical memory statistics\n");
    Console::print("snake :)  - play snake\n");
    Console::print("shell tty - start a new shell at terminal 'tty'\n");
    Console::print("exit      - exit current shell\n");
}

void list(const Vector<String>&) {
    auto files = System::files();
    auto dirs = System::subdirs();

    Console::print("Files: ");
    for (auto file : files) {
        Console::printf("%s ", file);
    }

    Console::print("\nDirectories: ");
    for (auto dir : dirs) {
        Console::printf("%s ", dir);
    }

    Console::print("\n");
}

void cd(const Vector<String>& parts) {
    if (parts.size() == 2) {
        System::swd(parts[1]);
    }
}

void view(const Vector<String>& parts) {
    if (parts.size() == 2) {
        Console::print(File::read(parts[1]));
    }
    Console::print("\n");
}

void type(const Vector<String>& parts) {
    if (parts.size() == 2) {
        Console::print("Write 'stop' to stop writing to file\n");

        String content;
        while (true) {
            Console::print("> ");
            auto line = Console::getLine();
            if (line == "stop")
                break;

            content += line + '\n';
        }

        if (File::write(parts[1], content)) {
            Console::printf("Written to file %s successfuly!\n", parts[1]);
        } else {
            Console::printf("Error writing to file %s!\n", parts[1]);
        }
    }
}

void kill(const Vector<String>& parts) {
    if (parts.size() == 2) {
        if (!System::kill(parts[1].toInt()))
            Console::printf("Can't kill kernel tasks!\n");
    }
}

void clear(const Vector<String>&) {
    Console::clear();
}

void date(const Vector<String>&) {
    auto datetime = System::date();
    Console::printf("%d.%d.%d %d:%d\n", datetime.day, datetime.month, datetime.year, datetime.hour, datetime.minutes);
}

void sleep(const Vector<String>& parts) {
    if (parts.size() == 2) {
        System::sleep(parts[1].toInt());
    }
}

void tasks(const Vector<String>&) {
    Console::print(File::read("/sys/tasks"));
}

void mem(const Vector<String>&) {
    Console::print(File::read("/sys/mem"));
}

void snake(const Vector<String>&) {
    int snake = System::exec("snake");
    System::wait(snake);
}

void shell(const Vector<String>& parts) {
    int tty = parts[1].toInt();
    if (tty == System::tty()) {
        Console::print("Can't start new shell in same terminal!\n");
    }

    System::exec("shell", tty);
}

void exit(const Vector<String>& parts) {
    System::exit();
}

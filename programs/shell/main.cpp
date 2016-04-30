#include <console.hpp>
#include <system.hpp>
#include <vector.hpp>
#include <file.hpp>

void main() {
    System::clear();
    Console::print("                            Welcome to Bug OS!\n", Console::Color::CYAN);
    Console::print("              * You can switch shells by pressing F1-F12 *\n\n", Console::Color::GREEN);

    Console::printf("Bug shell started (pid %d) \n", System::pid());
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

        auto cmd = parts[0];

        if (cmd == "help") {
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
            Console::print("exit      - exit current shell\n");
            Console::print("snake :)  - play snake\n");
        }

        if (cmd == "list") {
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
        } else if (cmd == "cd") {
            if (parts.size() == 2) {
                System::swd(parts[1]);
            }
        } else if (cmd == "view") {
            if (parts.size() == 2) {
                Console::print(File::read(parts[1]));
            }
            Console::print("\n");
        } else if (cmd == "type") {
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
        } else if (cmd == "kill") {
            if (parts.size() == 2) {
                if (!System::kill(parts[1].toInt()))
                    Console::printf("Can't kill kernel tasks!\n");
            }
        } else if (cmd == "clear") {
            Console::clear();
        } else if (cmd == "date") {
            auto datetime = System::date();
            Console::printf("%d.%d.%d %d:%d\n", datetime.day, datetime.month, datetime.year, datetime.hour, datetime.minutes);
        } else if (cmd == "sleep") {
            if (parts.size() == 2) {
                System::sleep(parts[1].toInt());
            }
        } else if (cmd == "tasks") {
            Console::print(File::read("/sys/tasks"));
        } else if (cmd == "mem") {
            Console::print(File::read("/sys/mem"));
        } else if (cmd == "snake") {
            int snake = System::exec("snake");
            System::wait(snake);
        } else if (cmd == "exit") {
            System::exit();
        }
    }
}

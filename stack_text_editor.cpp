#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

class DynamicStackBasedTextEditor
{
private:
    std::string text;
    std::stack<std::pair<std::string, std::string>> undoStack;
    std::stack<std::pair<std::string, std::string>> redoStack;
    std::string currentFile;

    std::vector<size_t> findOccurrences(const std::string &target)
    {
        std::vector<size_t> positions;
        size_t pos = text.find(target);
        while (pos != std::string::npos)
        {
            positions.push_back(pos);
            pos = text.find(target, pos + 1);
        }
        return positions;
    }

public:
    DynamicStackBasedTextEditor() : text(""), currentFile("") {}

    void createFile(const std::string &filePath)
    {
        if (std::filesystem::exists(filePath))
        {
            std::cout << "File '" << filePath << "' already exists.\n";
        }
        else
        {
            std::ofstream file(filePath);
            file.close();
            currentFile = filePath;
            text.clear();
            while (!undoStack.empty())
                undoStack.pop();
            while (!redoStack.empty())
                redoStack.pop();
            std::cout << "New file '" << filePath << "' created and loaded.\n";
        }
    }

    void loadFile(const std::string &filePath)
    {
        if (std::filesystem::exists(filePath))
        {
            std::ifstream file(filePath);
            text.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            currentFile = filePath;
            std::cout << "Loaded text from '" << filePath << "':\n"
                      << text << "\n";
        }
        else
        {
            std::cout << "File '" << filePath << "' not found.\n";
        }
    }

    void saveFile(const std::string &filePath = "")
    {
        if (!filePath.empty())
        {
            currentFile = filePath;
        }

        if (!currentFile.empty())
        {
            std::ofstream file(currentFile);
            file << text;
            file.close();
            std::cout << "Text saved to '" << currentFile << "'\n";
        }
        else
        {
            std::cout << "No file specified. Use 'save <filename>' to specify a file.\n";
        }
    }

    void insert(const std::string &newText)
    {
        std::string insertText = (text.empty() ? "" : " ") + newText;
        undoStack.push({"insert", insertText});
        while (!redoStack.empty())
            redoStack.pop();
        text += insertText;
        std::cout << "Inserted: '" << insertText << "' -> Current Text: '" << text << "'\n";
    }

    void deleteText(const std::string &textToDelete)
    {
        auto positions = findOccurrences(textToDelete);
        if (!positions.empty())
        {
            undoStack.push({"delete", textToDelete});
            while (!redoStack.empty())
                redoStack.pop();
            text.erase(std::remove(text.begin(), text.end(), textToDelete), text.end());
            std::cout << "Deleted all instances of: '" << textToDelete << "' -> Current Text: '" << text << "'\n";
        }
        else
        {
            std::cout << "Text '" << textToDelete << "' not found in current text.\n";
        }
    }

    void undo()
    {
        if (undoStack.empty())
        {
            std::cout << "Nothing to undo!\n";
            return;
        }

        auto action = undoStack.top();
        undoStack.pop();

        if (action.first == "insert")
        {
            text.erase(text.size() - action.second.size());
            redoStack.push(action);
            std::cout << "Undo Insert -> Current Text: '" << text << "'\n";
        }
        else if (action.first == "delete")
        {
            text += action.second;
            redoStack.push(action);
            std::cout << "Undo Delete -> Current Text: '" << text << "'\n";
        }
    }

    void redo()
    {
        if (redoStack.empty())
        {
            std::cout << "Nothing to redo!\n";
            return;
        }

        auto action = redoStack.top();
        redoStack.pop();

        if (action.first == "insert")
        {
            text += action.second;
            undoStack.push(action);
            std::cout << "Redo Insert -> Current Text: '" << text << "'\n";
        }
        else if (action.first == "delete")
        {
            text.erase(std::remove(text.begin(), text.end(), action.second), text.end());
            undoStack.push(action);
            std::cout << "Redo Delete -> Current Text: '" << text << "'\n";
        }
    }

    void displayText()
    {
        std::cout << "Current Text: '" << text << "'\n";
    }
};

int main()
{
    DynamicStackBasedTextEditor editor;
    std::cout << "Welcome to the Stack-Based Text Editor.\n";
    std::cout << "Commands: create <file>, load <file>, save <file>, insert <text>, delete <text>, undo, redo, display, quit\n";

    std::string command;
    while (true)
    {
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        if (command.rfind("create", 0) == 0)
        {
            editor.createFile(command.substr(7));
        }
        else if (command.rfind("load", 0) == 0)
        {
            editor.loadFile(command.substr(5));
        }
        else if (command.rfind("save", 0) == 0)
        {
            editor.saveFile(command.substr(5));
        }
        else if (command.rfind("insert", 0) == 0)
        {
            editor.insert(command.substr(7));
        }
        else if (command.rfind("delete", 0) == 0)
        {
            editor.deleteText(command.substr(7));
        }
        else if (command == "undo")
        {
            editor.undo();
        }
        else if (command == "redo")
        {
            editor.redo();
        }
        else if (command == "display")
        {
            editor.displayText();
        }
        else if (command == "quit")
        {
            break;
        }
        else
        {
            std::cout << "Invalid command. Try again.\n";
        }
    }

    return 0;
}

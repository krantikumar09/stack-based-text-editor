import os

class DynamicStackBasedTextEditor:
    def __init__(self):
        """
        Initializes the text editor with an empty string and two stacks:
        - undo_stack: to store actions for undo
        - redo_stack: to store actions for redo
        """
        self.text = ""
        self.undo_stack = []  
        self.redo_stack = []  
        self.current_file = None  

    def create_file(self, file_path):
        """
        Creates a new file if it doesn't exist and sets it as the current file.
        
        Parameters:
            file_path (str): Path to the file to be created.
        """
        if os.path.exists(file_path):
            print(f"File '{file_path}' already exists.")
        else:
            with open(file_path, 'w') as file:
                pass  
            self.current_file = file_path
            self.text = ""
            self.undo_stack.clear()
            self.redo_stack.clear()
            print(f"New file '{file_path}' created and loaded.")

    def load_file(self, file_path):
        if os.path.exists(file_path):
            with open(file_path, 'r') as file:
                self.text = file.read()
            self.current_file = file_path
            print(f"Loaded text from '{file_path}':\n{self.text}")
        else:
            print(f"File '{file_path}' not found.")

    def save_file(self, file_path=None):
        if file_path:
            self.current_file = file_path
        
        if self.current_file:
            with open(self.current_file, 'w') as file:
                file.write(self.text)
            print(f"Text saved to '{self.current_file}'")
        else:
            print("No file specified. Use 'save <filename>' to specify a file.")

    def insert(self, new_text):
        if self.text:
            new_text = " " + new_text

        self.undo_stack.append(("insert", new_text))
        self.redo_stack.clear()
        self.text += new_text
        print(f"Inserted: '{new_text}' -> Current Text: '{self.text}'")

    def delete_text(self, text_to_delete):
        if text_to_delete in self.text:
            occurrences = [pos for pos in range(len(self.text)) if self.text.startswith(text_to_delete, pos)]
            self.undo_stack.append(("delete_text", text_to_delete, occurrences))
            self.redo_stack.clear()
            
            self.text = self.text.replace(text_to_delete, "")
            print(f"Deleted all instances of: '{text_to_delete}' -> Current Text: '{self.text}'")
        else:
            print(f"Text '{text_to_delete}' not found in current text.")

    def undo(self):
        if not self.undo_stack:
            print("Nothing to undo!")
            return

        action = self.undo_stack.pop()
        
        if action[0] == "insert":
            content = action[1]
            self.text = self.text[:-len(content)]
            self.redo_stack.append(("insert", content))
            print(f"Undo Insert -> Current Text: '{self.text}'")
            
        elif action[0] == "delete_text":
            text_to_insert, positions = action[1], action[2]
            for pos in reversed(positions):
                self.text = self.text[:pos] + text_to_insert + self.text[pos:]
            self.redo_stack.append(("delete_text", text_to_insert, positions))
            print(f"Undo Delete Text -> Current Text: '{self.text}'")

    def redo(self):
        if not self.redo_stack:
            print("Nothing to redo!")
            return

        action = self.redo_stack.pop()
        
        if action[0] == "insert":
            content = action[1]
            self.text += content
            self.undo_stack.append(("insert", content))
            print(f"Redo Insert -> Current Text: '{self.text}'")
            
        elif action[0] == "delete_text":
            text_to_delete, positions = action[1], action[2]
            self.text = self.text.replace(text_to_delete, "")
            self.undo_stack.append(("delete_text", text_to_delete, positions))
            print(f"Redo Delete Text -> Current Text: '{self.text}'")

    def display_text(self):
        print(f"Current Text: '{self.text}'")


def main():
    editor = DynamicStackBasedTextEditor()
    
    print("Welcome to the Stack-Based Text Editor.")
    print("Commands: create <file>, load <file>, save <file>, insert <text>, delete_text <text>, undo, redo, display, quit")

    while True:
        command = input("Enter command: ").strip().split(" ", 1)
        action = command[0].lower()

        if action == "create" and len(command) > 1:
            file_path = command[1]
            editor.create_file(file_path)

        elif action == "load" and len(command) > 1:
            file_path = command[1]
            editor.load_file(file_path)

        elif action == "save" and len(command) > 1:
            file_path = command[1]
            editor.save_file(file_path)
        
        elif action == "save" and len(command) == 1:
            editor.save_file()

        elif action == "insert" and len(command) > 1:
            editor.insert(command[1])

        elif action == "delete_text" and len(command) > 1:
            editor.delete_text(command[1])

        elif action == "undo":
            editor.undo()

        elif action == "redo":
            editor.redo()

        elif action == "display":
            editor.display_text()

        elif action == "quit":
            break

        else:
            print("Invalid command. Try again.")


if __name__ == "__main__":
    main()

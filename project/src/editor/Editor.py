import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QSizePolicy
from CodeEditor import CodeEditor

class Editor(QMainWindow):
    def __init__(self, parent = None):
        QMainWindow.__init__(self, parent)
        self.setGeometry(100, 100, 1000, 500)

        editor = CodeEditor(self)
        self.setCentralWidget(editor)


def main():
    app = QApplication([])

    editor = Editor()
    editor.show()

    sys.exit(app.exec())

if __name__ == "__main__":
    main()
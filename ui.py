from PyQt5 import QtCore
from PyQt5.QtGui import QColor, QCursor, QFont
from PyQt5.QtWidgets import (QApplication, QGraphicsDropShadowEffect, 
                             QGroupBox,
                             QHBoxLayout,
                             QLabel,
                             QMessageBox, QScrollArea, QTabWidget, QVBoxLayout, QWidget)
import os
import sys
import json


class Parser:
    CONTENT = "content"
    FILENAME = "file_name"
    TOKEN_TYPE = "TokenType"
    VALUES = "values"
    LEXEME = "lexeme"
    LINE = "line"
    
    def __init__(self, folder="outputs"):
        CppCaller().execute()
        self.folderPath = os.path.join(
            os.path.abspath(os.path.dirname(__file__)), folder)
        if not os.path.exists(self.folderPath):
            raise Exception("Folder not exists")
        self.readingFiles()
    
    def readingFiles(self):
        import glob
        files = glob.glob(os.path.join(self.folderPath, "*.json"))
        self.files_content = []
        for file in files:
            name, _ = os.path.splitext(file)
            name = name.split(os.sep)[-1]
            content = None
            with open(file, "r") as f:
                content = json.loads(f.read())
            
            self.files_content.append({
                "file_name": name + '.json',
                "content": content,
                "order": int(name[-1])
            })
        
            
class CppCaller:
    def __init__(self) -> None:
        self.makefilePath = os.path.join(os.path.abspath(os.path.dirname(__file__)), "makefile")
        self.pathInputs = os.path.join(os.path.abspath(os.path.dirname(__file__)), "inputs")
        
    def getInputs(self):
        import glob 
        self.files = glob.glob(os.path.join(self.pathInputs, "*.txt"))
        
    def execute(self):
        self.getInputs()
        for file in self.files:
            name, ext = os.path.splitext(file)
            name = name.split(os.sep)[-1]
            os.system(f"make run ARG1={name}{ext} ARG2=output{name[-1]}.json")
    
    
class MainWindow(QWidget):
    def __init__(self, *args) -> None:
        super().__init__(*args)        
        self.parser = Parser()
        self.parser.readingFiles()
        self.qTabs = QTabWidget()
        self.mainLayout = QVBoxLayout()
        
        self.initUI()
        self.setMinimumWidth(1200)
        self.setMinimumHeight(800)
        self.mainLayout.addWidget(self.qTabs)
        self.setLayout(self.mainLayout)

    def initUI(self):
        for contnet in self.parser.files_content:
            self.qTabs.addTab(TabView(contnet), contnet[Parser.FILENAME])
        
        

    @staticmethod
    def errorHandler(text, title=""):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Critical)
        msg.setText("Warning!")
        msg.setInformativeText(text)
        msg.setWindowTitle(title)
        msg.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel)
        msg.exec_()


class TabView(QWidget):
    def __init__(self, data, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.data = data
        self.readingInputFile(self.data["order"])
        self.initUI()
        self.scrollLayout = QScrollArea()
        self.scrollLayout.setWidget(self.qGroupBox)
        self.qBox = QVBoxLayout()
        self.qBox.addWidget(self.scrollLayout)
        self.setLayout(self.qBox)
    
    def initUI(self):
        self.qGroupBox = QGroupBox("Tokens Lexemes")
        self.qGroupBox.setStyleSheet("background-color: #222222;")
        hBox = QHBoxLayout()
        
        for tokenType in self.data[Parser.CONTENT]:
            childGroup = QGroupBox(tokenType[Parser.TOKEN_TYPE])
            vbox = QVBoxLayout()
            # print(tokenType[Parser.VALUES])
            for val in tokenType[Parser.VALUES]:
                lexemeLabel = QLabel(val[Parser.LEXEME])
                lineLabel = QLabel(f"in line {val[Parser.LINE] + 1}")
                toolTipText = f"""
                                     <html> 
                                        <body> 
                                            <div style="padding: 10px">
                                                <h2> Line </h2> 
                                                <p> {self.input_lines[val[Parser.LINE]]} </p>
                                            </div>
                                        </body>
                                     </html>
                                """
                lineLabel.setToolTip(toolTipText)
                lexemeLabel.setToolTip(toolTipText)
                lexemeLabel.setCursor(QCursor(QtCore.Qt.PointingHandCursor))
                lineLabel.setCursor(QCursor(QtCore.Qt.PointingHandCursor))
                f = QFont("Helvetica [Cronyx]", 20, QFont.Bold)
                lexemeLabel.setFont(f)
                lineLabel.setFont(f)
                lineLabel.setStyleSheet("border-radius: 8px; font-size: 14px")
                lexemeLabel.setStyleSheet("border-radius: 8px; font-size: 14px")
                vboxChild = QVBoxLayout()
                childHbox = QHBoxLayout()
                childHbox.addWidget(lexemeLabel)
                childHbox.addStretch()   
                childHbox.addWidget(lineLabel)
                vboxChild.addLayout(childHbox)
                sh = QGraphicsDropShadowEffect()
                sh.setBlurRadius(15)
                sh.setOffset(0, 10.)
                sh.setColor(QColor(255, 255, 255, 15))
                ll = QLabel("------------------------------")
                ll.setGraphicsEffect(sh)
                ll.setStyleSheet("""color: rgba(255, 255, 255, 0.6); padding: 2px;
                                 border-radius: 6px; min-width: 100%; border: none;""")
                vboxChild.addWidget(ll) 
                vbox.addLayout(vboxChild)
                
            vbox.addStretch()
            childGroup.setLayout(vbox)
            childGroup.setStyleSheet("""margin: 3px; padding: 6px; margin-top: 5px;
                                     border: .5px solid #eee; border-radius: 6px;""")
            hBox.addWidget(childGroup)
        hBox.addStretch()
        self.qGroupBox.setLayout(hBox)
            
    def readingInputFile(self, order=1):
        path = os.path.join(os.path.abspath(os.path.dirname(__file__)), "inputs", f"input{order}.txt")
        try:
            with open(path, "r") as file:
                self.input_lines = file.readlines()
        except:
            print("folder is not found")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
    
    
            
        

#!/usr/bin/env python3

from PyQt5.QtWidgets import QApplication
from model import Model
from view import MainWindow
from controller import CalcController
from pathlib import Path

import sys

if __name__ == '__main__':
    app = QApplication([])

    try:
        fn = Path(__file__).parent / 'styles.qss'
        app.setStyleSheet(open(fn).read())
    except Exception as e:
        print('Failed to set stylesheet', e)

    model = Model()
    view = MainWindow()
    CalcController(model, view)

    view.show()
    sys.exit(app.exec_())


add_rules("mode.debug", "mode.release")

target("2DMainTone")
    add_rules("qt.widgetapp")
    add_headerfiles("src/*.h")
    add_files("src/*.cpp")
    add_files("src/*.ui")

    -- qt moc
    add_files("src/mainwindow.h")

    add_files("res.qrc")
    add_frameworks("QtCore", "QtWidgets", "QtGui")

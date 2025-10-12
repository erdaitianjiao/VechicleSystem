#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QPushButton>

class HomePage : public QWidget{

public:
    HomePage();

private:
   QPushButton *test1;

};

class MapPage : public QWidget {

public:
    MapPage();

private:
    QPushButton *test2;

};

#endif // PAGE_H

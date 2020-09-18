#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include "Stone.h"

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr);

    Stone _s[32];//定义32颗棋子

    int _r;//棋子的半径
    int _selectid;//记录被选中的棋子id
    bool _bRedTurn; //该轮到谁走了

    //返回象棋棋盘行列对应的像素坐标
    QPoint center (int row,int col);
    QPoint center (int id);

    void drawStone (QPainter& painter, int id);

    void paintEvent (QPaintEvent *);
    void mouseReleaseEvent (QMouseEvent *);

    bool getRowCol (QPoint pt,int& row,int& col);

    int getStoneId (int rEye,int cEye); //判断这个行列中有没有棋子

    bool isBottomSide (int id);

    //判断两个点之间有多少个棋子
    int getStoneCountAtLine (int row1,int col1,int row2,int col2);

    bool canMove (int moveid,int row,int col,int killid);
    bool canMove1 (int moveid,int row,int col,int killid);
    bool canMove2 (int moveid,int row,int col,int killid);
    bool canMove3 (int moveid,int row,int col,int killid);
    bool canMove4 (int moveid,int row,int col,int killid);
    bool canMove5 (int moveid,int row,int col,int killid);
    bool canMove6 (int moveid,int row,int col,int killid);
    bool canMove7 (int moveid,int row,int col,int killid);

signals:

public slots:
};

#endif // BOARD_H

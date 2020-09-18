#include "Board.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

Board::Board(QWidget *parent) : QWidget(parent)
{
    //初始化棋子
     for(int i=0;i<32;i++)
     {
         _s[i].init(i);
     }

     _selectid = -1;
     _bRedTurn = true;
}

//画棋盘
void Board::paintEvent(QPaintEvent *)
{
    QPainter painter (this);
    int d=40;
    _r=d/2;
    //画10条横线
    for (int i=1;i<=10;i++)
    {
        painter.drawLine(QPoint(d,i*d),QPoint(9*d,i*d));
    }
    //画9条竖线
    for (int i=1;i<=9;i++)
    {
        //楚河汉界
        if (i==1 || i==9)
            painter.drawLine(QPoint(i*d,d),QPoint(i*d,10*d));
        else
        {
            painter.drawLine(QPoint(i*d,d),QPoint(i*d,5*d));
            painter.drawLine(QPoint(i*d,6*d),QPoint(i*d,10*d));
        }
    }

    //画九宫格
    painter.drawLine(QPoint(4*d,d),QPoint(6*d,3*d));
    painter.drawLine(QPoint(6*d,d),QPoint(4*d,3*d));
    painter.drawLine(QPoint(4*d,8*d),QPoint(6*d,10*d));
    painter.drawLine(QPoint(6*d,8*d),QPoint(4*d,10*d));

    //绘制32颗棋子
   for (int i=0;i<32;++i)
   {
       drawStone (painter,i);
   }
}

//通过行和列来获取像素坐标
QPoint Board::center(int row, int col)
{
    QPoint ret;
    ret.rx() = (col+1)*_r*2;
    ret.ry() = (row+1)*_r*2;
    return ret;
}

//重载函数，防止调用时太过于繁琐
QPoint Board::center(int id)
{
   return center(_s[id]._row,_s[id]._col);
}

//绘制棋子
void Board::drawStone(QPainter& painter,int id)
{
    if (_s[id]._dead)
        return;
    QPoint c = center (id);
   QRect rect = QRect(c.x()-_r,c.y()-_r,_r*2,_r*2);//绘制了一个写字的矩形 
   //设置画刷，给棋子上色
   if (id == _selectid)
       painter.setBrush(QBrush(Qt::gray)); //被选中的棋子涂成灰色
   else
       painter.setBrush(QBrush(Qt::yellow));//其他棋子涂成黄色

   painter.setPen(Qt::black);//这个画笔是来画矩形的

   painter.drawEllipse(center(id),_r,_r);//画圆

   //给棋子上的字涂上颜色，来区分红黑子
   if (_s[id]._red)
     painter.setPen(Qt::red);
   painter.setFont(QFont("system",_r,700));

   painter.drawText(rect,_s[id].getText(),QTextOption(Qt::AlignCenter));//在棋子上写字
}

bool Board::getRowCol(QPoint pt, int &row, int &col)
{
    //遍历行和列，从而获取行列坐标
    for(row=0;row<=9;row++)
    {
        for (col=0;col<=8;col++)
        {
            QPoint c= center(row,col);//获取像素坐标
            int dx=c.x()-pt.x();
            int dy=c.y()-pt.y();
            int dist = dx*dx+dy*dy;
            if (dist<_r*_r)
                return true;
        }
    }
    return false;
}
//将的移动
bool Board::canMove1 (int moveid,int row,int col,int killid)
{
    /*
      1.首先目标位置在九宫格内
      2.移动的步长是一个格子
    */
    //只能在九宫格内走动
    if (_s[moveid]._red)
    {
        if (row<7) return false;
    }
    else
    {
        if (row>2) return false;
    }
    if (col <3) return false;
    if (col >5) return false;

    //因为将只走一格，所以要么是行上面相差一格，要么是列上面相差一格
    //所以dr和dc中有一个为1，有一个为0
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d =abs(dr)*10 + abs(dc);
    if (d != 1 && d!=10)
        return false;
    return true;
}
//士的移动
bool Board::canMove2 (int moveid,int row,int col,int )
{   if (_s[moveid]._red)
    {
        if (row<7) return false;
    }
    else
    {
        if (row>2) return false;
    }
    if (col <3) return false;
    if (col >5) return false;
    //以上为九宫格内判断

    //因为士是走对角线的
    //所以行列都差一格，所以dr和dc都为1
    int dr = _s[moveid]._row - row;
    int dc = _s[moveid]._col - col;
    int d =abs(dr)*10 + abs(dc);  //因为士走对角线，所以dr和dc都为1
    if (d == 11)
        return true;
    return false;
}
//象的移动
bool Board::canMove3 (int moveid,int row,int col,int)
{
    //因为象走田，所以行和列都相差两格
    //所以dc和dr都为2
     int dr = _s[moveid]._row - row;
     int dc = _s[moveid]._col - col;
     int d = abs(dr)*10 + abs(dc);
     if (d != 22) return false;
     //象眼所在行列
     int rEye = (row+_s[moveid]._row)/2;
     int cEye = (col+_s[moveid]._col)/2;
     //判断象眼上是否有棋子
     if (getStoneId(rEye,cEye) != -1)
         return false;
     //判断棋子在棋盘上方还是下方
     if (isBottomSide(moveid))
     {
         if (row<4) return false; //棋子在下方，不能过河
     }
     else
     {
         if (row>5) return  false;
     }
     return true;
}
//车的移动
bool Board::canMove4 (int moveid,int row,int col,int )
{
    if (getStoneCountAtLine(_s[moveid]._row,_s[moveid]._col,row,col)==0)
     return true;
    return false;
}
bool Board::canMove5 (int moveid,int row,int col,int )
{
    //马走日,所以dr=2，dc=1或者dr=1，dc=2
    //所以d=21/12
    int dr=_s[moveid]._row-row;
    int dc=_s[moveid]._col-col;
    int d = abs(dr)*10+abs(dc);
    if (d != 21 && d!= 12)
        return false;
    if (dc==2)
    {
        int ret = getStoneId(_s[moveid]._row,_s[moveid]._col-1);
        if (ret != -1)
            return false;
    }
    if (dc==-2)
    {
        int ret = getStoneId(_s[moveid]._row,_s[moveid]._col+1);
        if (ret != -1)
            return false;
    }
    if (dr==2)
    {
        int ret = getStoneId(_s[moveid]._row-1,_s[moveid]._col);
        if (ret != -1)
            return false;
    }
    if (dr==-2)
    {
        int ret = getStoneId(_s[moveid]._row+1,_s[moveid]._col);
        if (ret != -1)
            return false;
    }
     return true;
}
//炮的移动
bool Board::canMove6 (int moveid,int row,int col,int killid)
{
    int ret = getStoneCountAtLine(_s[moveid]._row,_s[moveid]._col,row,col);
    if (killid != -1)
    {
        //当killid!=-1时，即炮要吃子时
        //只能隔一个走
        if (ret == 1) return true;
    }
    else
    {
        //当炮不吃子时，只能走直线
        if (ret == 0) return true;
    }
    return false;
}
bool Board::canMove7 (int moveid,int row,int col,int)
{
    //兵只能走一格，所以dr=0，dc=1或者dr=1，dc=0
    //d=10或者1
     int dr = _s[moveid]._row - row;
     int dc = _s[moveid]._col - col;
     int d = abs(dr)*10+abs(dc);
     if (d!=10&&d!=1)
         return false;
     if (isBottomSide(moveid))
     {
         if (row>_s[moveid]._row) return false;
         if (row>=5 && row == _s[moveid]._row) return false;
     }
     else
     {
         if (row<_s[moveid]._row) return false;
         if (row<=4&& row == _s[moveid]._row)  return false;
     }
     return true;
}

bool Board::canMove(int moveid, int row, int col, int killid)
{
    if (killid!=-1)
    {
    if (_s[moveid]._red == _s[killid]._red)//moveid和killid颜色相同
    {
        _selectid=killid ; //killid是第二次选中的棋子，当都是一边时，就相当于重新选了一次棋子
        update();

        return false;
    }
    }
    switch (_s[moveid]._type)
    {
    case Stone::JIANG:
        return canMove1(moveid,row,col,killid);
    case Stone::SHI:
        return canMove2(moveid,row,col,killid);
    case Stone::XIANG:
        return canMove3(moveid,row,col,killid);
    case Stone::CHE:
        return canMove4(moveid,row,col,killid);
    case Stone::MA:
        return canMove5(moveid,row,col,killid);
    case Stone::PAO:
        return canMove6(moveid,row,col,killid);
    case Stone::BING:
        return canMove7(moveid,row,col,killid);
    }

    return true;
}

//每释放一次鼠标，都会调用一次这个函数
void Board::mouseReleaseEvent(QMouseEvent *ev)
{
   QPoint pt = ev->pos();//得到鼠标点击的位置
   // 将pt转化成象棋的行列
   //判断这个行列值有没有棋子
   int row,col;
   bool bRet = getRowCol(pt,row,col);
   if (bRet == false) //点到棋盘外了，就不会有反应
       return;

   int i;
   int clickid=-1;
   for ( i=0;i<32;i++)
   {
       if (_s[i]._col==col&&_s[i]._row==row&&_s[i]._dead==false)
       {
           break;
       }
   }
   //以上有两种情况，一为没有选到棋子，二为选到棋子了，这时i<32
   if (i<32)
   {
      clickid = i;  //记录被选中棋子的id
   }

   if (_selectid==-1) //当没有棋子被选中时
   {
   if (clickid != -1) //当前位置选的是棋子时
   {
       if (_bRedTurn == _s[clickid]._red)
       {
          _selectid = clickid; //将被选中的棋子id传给_selectid
          update(); //重绘棋面
       }
   }
   }
   else {
       //走棋的代码
       if (canMove(_selectid,row,col,clickid))
       {
           _s[_selectid]._row = row;
           _s[_selectid]._col = col;
           if (clickid!=-1)
           {
               _s[clickid]._dead=true;
           }
           _selectid=-1;
           _bRedTurn = !_bRedTurn;
           update();
       }
   }
}

int Board::getStoneId(int rEye, int cEye)
{
    for (int i=0;i<32;i++)
    {
        if (_s[i]._col==cEye&&_s[i]._row==rEye)
            return i;
    }
    return -1;
}

bool Board::isBottomSide(int id)
{
    if (_s[id]._red) return true;
    return false;
}

int Board::getStoneCountAtLine(int row1, int col1, int row2, int col2)
{
    int ret = 0;
    //如果棋子和目的地不在同一行或者同一列，就返回-1
    if (row1 != row2&&col1 != col2)
        return -1;
    //如果棋子和目的地在一个地方，也返回-1
    if (row1 == row2&&col1 == col2)
        return -1;

    if (row1==row2)
    {
        int min = col1 < col2 ? col1 : col2;
        int max = col1 < col2 ? col2 : col1;
        //在同一行中 遍历列 注意下面的min+1
        //如果没有+1，就会把棋子本身的数量也统计进去
        for (int col = min+1;col<max;++col)
        {
            //有一个棋子，就++
            if (getStoneId(row1,col) != -1) ++ret;
        }
    }
    else
    {
        int min = row1 < row2 ? row1 : row2;
        int max = row1 < row2 ? row2 : row1;
        //在同一列中，遍历行
        for  (int row = min+1;row<max;row++)
        {
            if (getStoneId(row,col1) != -1) ++ret;
        }
    }
    return  ret;
}

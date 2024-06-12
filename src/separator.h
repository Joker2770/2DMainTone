/*************************************************************************
    > File Name: separator.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: 2024年06月12日 星期三 09时27分43秒
 ************************************************************************/

#ifndef __SEPARATOR_H__
#define __SEPARATOR_H__

#include <vector>
#include <QPoint>

class Separator
{
public:
    Separator();
    virtual ~Separator();

    std::vector<std::pair<QPoint, QPoint>> &get_coordinate_separation_with_2D(unsigned int width, unsigned int height, unsigned int row, unsigned int column);

private:
    std::vector<std::pair<QPoint, QPoint>> coordinate_list;
};

#endif

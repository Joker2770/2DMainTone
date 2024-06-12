/*************************************************************************
    > File Name: separator.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com
    > Created Time: 2024年06月12日 星期三 09时27分48秒
 ************************************************************************/

#include"separator.h"

Separator::Separator()
{
    this->coordinate_list.clear();
}

Separator::~Separator()
{
    this->coordinate_list.clear();
}

std::vector<std::pair<QPoint, QPoint>>& Separator::get_coordinate_separation_with_2D(unsigned int width, unsigned int height, unsigned int row, unsigned int column)
{
    this->coordinate_list.clear();
    if (row < 1 || row >= height || column < 1 || column >= width)
    {
        QPoint ps(0,0);
        QPoint pe(width, height);
        std::pair<QPoint, QPoint> p(ps, pe);
        this->coordinate_list.push_back(p);
    }
    else
    {
        unsigned int u_height_unit= height / row;
        unsigned int u_width_unit= width / column;
        for (unsigned int i = 0; i < row; ++i) {
            for (unsigned int j = 0; j < column; ++j) {
                QPoint ps(j * u_width_unit, i * u_height_unit);
                unsigned int pe_x = (j + 1) * u_width_unit;
                unsigned int pe_y = (i + 1) * u_height_unit;
                if ((i + 1) * u_height_unit >= height)
                {
                    pe_y = height - 1;
                }
                if ((j + 1) * u_width_unit >= width)
                {
                    pe_x = width -1;
                }
                QPoint pe(pe_x, pe_y);
                std::pair<QPoint, QPoint> p(ps, pe);
                this->coordinate_list.push_back(p);
            }
        }
    }
    return this->coordinate_list;
}


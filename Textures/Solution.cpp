#include "Common.h"

#include <memory>
#include <algorithm>

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`

class Rectangle : public IShape {
public:
    Rectangle(ShapeType type) : _type(type) {}

    std::unique_ptr<IShape> Clone() const override {
        auto shape = make_unique<Rectangle>(_type);
        shape->SetSize(_size);
        shape->SetPosition(_point);
        shape->SetTexture(_texture);
        return std::move(shape);
    }

    void SetPosition(Point p) override {
        _point = p;
    }

    Point GetPosition() const override {
        return _point;
    }

    void SetSize(Size s) override {
        _size = s;
    }

    Size GetSize() const override {
        return _size;
    }

    void SetTexture(std::shared_ptr<ITexture> tex) override {
        _texture = move(tex);
    }

    ITexture* GetTexture() const override {
        return _texture.get();
    }

    void Draw(Image& image) const override {
        if(image.size() == 0 || image[0].size() == 0)
            return;

        size_t h = std::min(image.size(), (size_t)(_point.y + _size.height));
        size_t w = std::min(image[0].size(), (size_t)(_point.x + _size.width));

        for(size_t y = _point.y; y < h; ++y) {
            for(size_t x = _point.x; x < w; ++x) {
                if(IsTexture(x, y)) {
                    char newCh = _texture->GetImage()[y - _point.y][x - _point.x];
                    if(_type == ShapeType::Rectangle) {
                        image[y][x] = newCh;
                    } else {
                        if(IsPointInEllipse({(int)(x - _point.x),(int)(y - _point.y)}, _size))
                            image[y][x] = newCh;;
                    }
                } else {
                    char newCh = '.';
                    if(_type == ShapeType::Rectangle)
                        image[y][x] = newCh;
                    else if(IsPointInEllipse({(int)(x - _point.x),(int)(y - _point.y)}, _size))
                        image[y][x] = newCh;
                }
            }
        }
    }

private:
    bool IsTexture(int x, int y) const {
        return _texture != nullptr &&
               y < _texture->GetSize().height + _point.y &&
               x < _texture->GetSize().width + _point.x;
    }

    Point _point;
    Size _size;
    std::shared_ptr<ITexture> _texture;
    ShapeType _type;
};

//class Ellipse : public IShape {
//public:
//    std::unique_ptr<IShape> Clone() const override {
//
//    }
//
//   void SetPosition(Point) override {
//
//    }
//
//    Point GetPosition() const override {
//
//    }
//
//    void SetSize(Size) override {
//
//    }
//
//    Size GetSize() const override {
//
//    }
//
//    void SetTexture(std::shared_ptr<ITexture>) override {
//
//    }
//    ITexture* GetTexture() const override {
//
//    }
//
//    void Draw(Image&) const override {
//
//    }
//
//private:
//    Point _point;
//    Size _size;
//    std::shared_ptr<ITexture> _texture;
//};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
    return make_unique<Rectangle>(shape_type);
}
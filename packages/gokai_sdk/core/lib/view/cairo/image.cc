#include <gokai/view/cairo/image.h>

#define TAG "Gokai::View::Cairo::Image"

using namespace Gokai::View::Cairo;

Image::Image(Gokai::ObjectArguments arguments) : Gokai::View::Displayable(arguments), Gokai::Loggable(TAG, arguments) {
  this->value = cairo_surface_reference(std::any_cast<cairo_surface_t*>(arguments.get("value")));
}

Image::~Image() {
  //cairo_surface_destroy(this->value);
}

cairo_surface_t* Image::getValue() {
  return this->value;
}

Gokai::View::URect Image::getRect() {
  return Gokai::View::URect(
    glm::uvec2(0, 0),
    glm::uvec2(
      cairo_image_surface_get_width(this->value),
      cairo_image_surface_get_height(this->value)
    )
  );
}
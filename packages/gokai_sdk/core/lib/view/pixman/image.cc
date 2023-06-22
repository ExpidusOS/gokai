#include <gokai/view/pixman/image.h>

#define TAG "Gokai::View::Pixman::Image"

using namespace Gokai::View::Pixman;

Image::Image(Gokai::ObjectArguments arguments) : Gokai::View::Displayable(arguments), Gokai::Loggable(TAG, arguments) {
  this->value = pixman_image_ref(std::any_cast<pixman_image_t*>(arguments.get("value")));
}

Image::~Image() {
  pixman_image_unref(this->value);
}

pixman_image_t* Image::getValue() {
  return this->value;
}

Gokai::View::URect Image::getRect() {
  return Gokai::View::URect(
    glm::uvec2(0, 0),
    glm::uvec2(
      pixman_image_get_width(this->value),
      pixman_image_get_height(this->value)
    )
  );
}
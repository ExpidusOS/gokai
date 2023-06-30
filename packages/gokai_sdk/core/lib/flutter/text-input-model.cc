#include <gokai/flutter/text-input-model.h>
#include <algorithm>
#include <codecvt>
#include <locale>

#define IS_LEADING_SURROGATE(code_point) (((code_point) & 0xFFFFFC00) == 0xD800)
#define IS_TRAILING_SURROGATE(code_point) (((code_point) & 0xFFFFFC00) == 0xDC00)

using namespace Gokai::Flutter;

TextInputModel::TextInputModel() : selection(0), composing_range(0), is_composing(false) {}

bool TextInputModel::setText(const std::string& text, const TextRange& selection, const TextRange& composing_range) {
  this->text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text);

  if (!this->getTextRange().contains(selection) || !this->getTextRange().contains(composing_range)) {
    return false;
  }

  this->selection = selection;
  this->composing_range = composing_range;
  this->is_composing = !composing_range.isCollapsed();
  return true;
}

bool TextInputModel::setSelection(const TextRange& range) {
  if (this->is_composing && !range.isCollapsed()) return false;
  if (!this->getEditableRange().contains(range)) return false;
  this->selection = range;
  return true;
}

bool TextInputModel::setComposingRange(const TextRange& range, size_t offset) {
  if (!this->is_composing || !this->getTextRange().contains(range)) return false;

  this->composing_range = range;
  this->selection = TextRange(range.getStart() + offset);
  return true;
}

void TextInputModel::beginComposing() {
  this->is_composing = true;
  this->composing_range = TextRange(this->selection.getStart());
}

void TextInputModel::updateComposingText(const std::u16string& text) {
  if (text.length() == 0 && this->composing_range.isCollapsed()) return;

  this->deleteSelected();
  this->text.replace(this->composing_range.getStart(), this->composing_range.getLength(), text);
  this->composing_range.setEnd(this->composing_range.getStart() + text.length());
  this->selection = TextRange(this->composing_range.getEnd());
}

void TextInputModel::updateComposingText(const std::string& text) {
  this->updateComposingText(std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text));
}

void TextInputModel::commitComposing() {
  if (this->composing_range.isCollapsed()) return;

  this->composing_range = TextRange(this->composing_range.getEnd());
  this->selection = this->composing_range;
}

void TextInputModel::endComposing() {
  this->is_composing = false;
  this->composing_range = TextRange(0);
}

bool TextInputModel::deleteSelected() {
  if (this->selection.isCollapsed()) return false;

  size_t start = this->selection.getStart();
  this->text.erase(start, this->selection.getLength());
  this->selection = TextRange(start);

  if (this->is_composing) {
    this->composing_range = this->selection;
  }
  return true;
}

void TextInputModel::addCodePoint(char32_t c) {
  if (c <= 0xFFFF) {
    this->addText(std::u16string({ static_cast<char16_t>(c) }));
  } else {
    char32_t to_decompose = c - 0x10000;
    this->addText(std::u16string({
      static_cast<char16_t>((to_decompose >> 10) + 0xd800),
      static_cast<char16_t>((to_decompose % 0x400) + 0xdc00),
    }));
  }
}

void TextInputModel::addText(const std::u16string& text) {
  this->deleteSelected();

  if (this->is_composing) {
    this->text.erase(this->composing_range.getStart(), this->composing_range.getLength());
    this->selection = TextRange(this->composing_range.getStart());
    this->composing_range = TextRange(this->composing_range.getStart() + text.length());
  }

  size_t pos = this->selection.getPosition();
  this->text.insert(pos, text);
  this->selection = TextRange(pos + text.length());
}

void TextInputModel::addText(const std::string& text) {
  this->addText(std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text));
}

bool TextInputModel::performBackspace() {
  if (this->deleteSelected()) return true;

  size_t pos = this->selection.getPosition();
  if (pos != this->getEditableRange().getStart()) {
    int count = IS_TRAILING_SURROGATE(this->text.at(pos - 1)) ? 2 : 1;

    this->text.erase(pos - static_cast<size_t>(count), static_cast<size_t>(count));
    this->selection = TextRange(pos - static_cast<size_t>(count));

    if (this->is_composing) {
      this->composing_range.setEnd(this->composing_range.getEnd() - static_cast<size_t>(count));
    }
    return true;
  }
  return false;
}

bool TextInputModel::performDelete() {
  if (this->deleteSelected()) return true;

  size_t pos = this->selection.getPosition();
  if (pos < this->getEditableRange().getEnd()) {
    int count = IS_LEADING_SURROGATE(this->text.at(pos - 1)) ? 2 : 1;

    this->text.erase(pos, static_cast<size_t>(count));

    if (this->is_composing) {
      this->composing_range.setEnd(this->composing_range.getEnd() - static_cast<size_t>(count));
    }
    return true;
  }
  return false;
}

bool TextInputModel::deleteSurrounding(int offset, int count) {
  size_t max_pos = this->getEditableRange().getEnd();
  size_t start = this->selection.getExtent();

  if (offset < 0) {
    for (int i = 0; i < -offset; i++) {
      if (start == this->getEditableRange().getStart()) {
        count = i;
        break;
      }

      start -= IS_TRAILING_SURROGATE(this->text.at(start - 1)) ? 2 : 1;
    }
  } else {
    for (int i = 0; i < offset && start != max_pos; i++) {
      start += IS_LEADING_SURROGATE(this->text.at(start)) ? 2 : 1;
    }
  }

  auto end = start;
  for (int i = 0; i < count && end != max_pos; i++) {
    end += IS_LEADING_SURROGATE(this->text.at(start)) ? 2 : 1;
  }

  if (start == end) return false;

  auto deleted_length = end - start;
  this->text.erase(start, deleted_length);
  this->selection = TextRange(offset <= 0 ? start : this->selection.getStart());

  if (this->is_composing) {
    this->composing_range.setEnd(this->composing_range.getEnd() - deleted_length);
  }
  return true;
}

bool TextInputModel::moveCursorToBeginning() {
  size_t min_pos = this->getEditableRange().getStart();
  if (this->selection.isCollapsed() && this->selection.getPosition() == min_pos) return false;

  this->selection = TextRange(min_pos);
  return true;
}

bool TextInputModel::moveCursorToEnd() {
  size_t max_pos = this->getEditableRange().getEnd();
  if (this->selection.isCollapsed() && this->selection.getPosition() == max_pos) return false;

  this->selection = TextRange(max_pos);
  return true;
}

bool TextInputModel::selectToBeginning() {
  size_t min_pos = this->getEditableRange().getStart();
  if (this->selection.isCollapsed() && this->selection.getPosition() == min_pos) return false;

  this->selection = TextRange(this->selection.getBase(), min_pos);
  return true;
}

bool TextInputModel::selectToEnd() {
  size_t max_pos = this->getEditableRange().getEnd();
  if (this->selection.isCollapsed() && this->selection.getPosition() == max_pos) return false;

  this->selection = TextRange(this->selection.getBase(), max_pos);
  return true;
}

bool TextInputModel::moveCursorForward() {
  if (!this->selection.isCollapsed()) {
    this->selection = TextRange(this->selection.getEnd());
    return true;
  }

  size_t pos = this->selection.getPosition();
  if (pos != this->getEditableRange().getEnd()) {
    int count = IS_LEADING_SURROGATE(this->text.at(pos)) ? 2 : 1;
    this->selection = TextRange(pos + static_cast<size_t>(count));
    return true;
  }
  return false;
}

bool TextInputModel::moveCursorBack() {
  if (!this->selection.isCollapsed()) {
    this->selection = TextRange(this->selection.getStart());
    return true;
  }

  size_t pos = this->selection.getPosition();
  if (pos != this->getEditableRange().getEnd()) {
    int count = IS_TRAILING_SURROGATE(this->text.at(pos - 1)) ? 2 : 1;
    this->selection = TextRange(pos - static_cast<size_t>(count));
    return true;
  }
  return false;
}

std::string TextInputModel::getText() const {
  return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(text);
}

int TextInputModel::getCursorOffset() const {
  auto leading_text = this->text.substr(0, this->selection.getExtent());
  return static_cast<int>(std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(leading_text).size());
}

TextRange TextInputModel::getTextRange() const {
  return TextRange(0, this->text.length());
}

TextRange TextInputModel::getSelection() const {
  return this->selection;
}

TextRange TextInputModel::getComposingRange() const {
  return this->composing_range;
}

bool TextInputModel::isComposing() const {
  return this->is_composing;
}

TextRange TextInputModel::getEditableRange() const {
  return this->is_composing ? this->composing_range : this->getTextRange();
}
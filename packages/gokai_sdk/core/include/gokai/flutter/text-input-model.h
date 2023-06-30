#pragma once

#ifdef __cplusplus
#include <gokai/flutter/text-range.h>
#include <gokai/object.h>
#include <string>

namespace Gokai {
  namespace Flutter {
    class TextInputModel {
      public:
        TextInputModel();

        bool setText(
          const std::string& text,
          const TextRange& selection = TextRange(0),
          const TextRange& composing_range = TextRange(0)
        );

        bool setSelection(const TextRange& range);
        bool setComposingRange(const TextRange& range, size_t offset);

        void beginComposing();
        void updateComposingText(const std::u16string& text);
        void updateComposingText(const std::string& text);
        void commitComposing();
        void endComposing();
        bool deleteSelected();

        void addCodePoint(char32_t c);

        void addText(const std::u16string& text);
        void addText(const std::string& text);

        bool performBackspace();
        bool performDelete();
        bool deleteSurrounding(int offset, int count);

        bool moveCursorToBeginning();
        bool moveCursorToEnd();

        bool selectToBeginning();
        bool selectToEnd();

        bool moveCursorForward();
        bool moveCursorBack();

        std::string getText() const;
        int getCursorOffset() const;
        TextRange getTextRange() const;
        TextRange getSelection() const;
        TextRange getComposingRange() const;
        bool isComposing() const;
      private:
        std::u16string text;
        TextRange selection;
        TextRange composing_range;
        bool is_composing;

        TextRange getEditableRange() const;
    };
  }
}
#endif
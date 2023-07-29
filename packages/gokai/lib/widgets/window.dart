import 'package:arena_listener/arena_listener.dart';
import 'package:event/event.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import 'package:gokai/view/window.dart';
import 'package:gokai/services.dart';

class GokaiWindowView extends StatefulWidget {
  const GokaiWindowView({
    super.key,
    required this.id,
    required this.windowManager,
    this.inputManager,
    this.interactive = true,
    this.size,
    this.decorationBuilder,
    this.errorBuilder,
    this.progressBuilder,
  });

  final String id;
  final GokaiWindowManager windowManager;
  final GokaiInputManager? inputManager;
  final Size? size;
  final bool interactive;
  final Widget Function(BuildContext context, Widget child, GokaiWindow win)? decorationBuilder;
  final Widget Function(BuildContext context, Object error)? errorBuilder;
  final WidgetBuilder? progressBuilder;

  @override
  State<GokaiWindowView> createState() => _GokaiWindowViewState();
}

class _GokaiWindowViewState extends State<GokaiWindowView> {
  UniqueKey key = UniqueKey();
  int _buttons = 0;

  void _onMapped(Value<String>? id) {
    if (id!.value == widget.id) {
      setState(() {
        key = UniqueKey();
      });
    }
  }

  void _onCommit(Value<String>? id) {
    if (id!.value == widget.id) {
      setState(() {
        key = UniqueKey();
      });
    }
  }

  @override
  void initState() {
    super.initState();

    widget.windowManager.onMapped.subscribe(_onMapped);
    widget.windowManager.onCommit.subscribe(_onCommit);
  }

  @override
  void dispose() {
    super.dispose();

    widget.windowManager.onMapped.unsubscribe(_onMapped);
    widget.windowManager.onCommit.unsubscribe(_onCommit);
  }

  bool? _updateMouseButtons(int newButtons) {
    if (_buttons == newButtons) return null;

    var button = _buttons ^ newButtons;
    var state = newButtons & button != 0 ? false : true;
    _buttons = newButtons;
    return state;
  }

  bool _handleMouseButtons(String windowId, int btns, double x, double y) {
    var isReleased = _updateMouseButtons(btns);
    if (isReleased != null) {
      widget.inputManager!.sendPointer(GokaiPointerEvent(
        type: GokaiPointerEventType.button,
        windowId: windowId,
        x: x,
        y: y,
        button: _buttons,
        isReleased: isReleased
      ));
      return true;
    }
    return false;
  }

  void _mouseMove(String windowId, double x, double y) {
    widget.inputManager!.sendPointer(GokaiPointerEvent(
      type: GokaiPointerEventType.hover,
      windowId: windowId,
      x: x,
      y: y,
      button: _buttons,
      isReleased: true,
    ));
  }

  @override
  Widget build(BuildContext context) =>
    FutureBuilder(
      future: widget.windowManager.get(widget.id),
      builder: (context, snapshot) {
        if (snapshot.hasError) {
          if (widget.errorBuilder != null) {
            return widget.errorBuilder!(context, snapshot.error!);
          }
          return Text('Failed to create window view: "${widget.id}"');
        }

        if (snapshot.hasData) {
          if (snapshot.data!.texture != null) {
            final size = widget.size
              ?? (snapshot.data!.rect.size.isEmpty
                ? MediaQuery.sizeOf(context)
                : snapshot.data!.rect.size);

            Widget child = SizedBox.fromSize(
              size: size,
              child: Stack(
                clipBehavior: Clip.none,
                children: [
                  ...snapshot.data!.childrenBelow.map(
                    (child) => GokaiWindowView(
                      id: child,
                      windowManager: widget.windowManager,
                      interactive: false,
                    ),
                  ).toList(),
                  Texture(
                    filterQuality: FilterQuality.medium,
                    textureId: snapshot.data!.texture!,
                  ),
                  ...snapshot.data!.childrenAbove.map(
                    (child) => GokaiWindowView(
                      id: child,
                      windowManager: widget.windowManager,
                      interactive: false,
                    ),
                  ).toList(),
                ],
              ),
            );

            if (widget.interactive) {
              if (widget.inputManager != null) {
                child = ArenaListener(
                  onPointerUp: (event, disposition) {
                    if (disposition == GestureDisposition.accepted) return null;

                    if (event.kind == PointerDeviceKind.mouse) {
                      return _handleMouseButtons(snapshot.data!.id, event.buttons, event.position.dx, event.position.dy)
                        ? GestureDisposition.accepted
                        : GestureDisposition.rejected;
                    }

                    if (event.kind == PointerDeviceKind.touch) {
                      widget.inputManager!.sendTouch(GokaiTouchEvent(
                        type: GokaiTouchEventType.up,
                        windowId: snapshot.data!.id,
                        id: event.pointer,
                        x: event.position.dx,
                        y: event.position.dy,
                      ));
                    }
                    return GestureDisposition.rejected;
                  },
                  onPointerDown: (event) {
                    if (event.kind == PointerDeviceKind.mouse) {
                      final result = _handleMouseButtons(snapshot.data!.id, event.buttons, event.position.dx, event.position.dy)
                        ? GestureDisposition.accepted
                        : GestureDisposition.rejected;
                      _mouseMove(snapshot.data!.id, event.position.dx, event.position.dy);
                      return result;
                    }

                    if (event.kind == PointerDeviceKind.touch) {
                      widget.inputManager!.sendTouch(GokaiTouchEvent(
                        type: GokaiTouchEventType.down,
                        windowId: snapshot.data!.id,
                        id: event.pointer,
                        x: event.position.dx,
                        y: event.position.dy,
                      ));
                    }
                    return GestureDisposition.rejected;
                  },
                  onPointerCancel: (event, disposition) {
                    return GestureDisposition.rejected;
                  },
                  onPointerMove: (event, disposition) {
                    if (event.kind == PointerDeviceKind.mouse) {
                      final result = _handleMouseButtons(snapshot.data!.id, event.buttons, event.position.dx, event.position.dy)
                        ? GestureDisposition.accepted
                        : GestureDisposition.rejected;
                      _mouseMove(snapshot.data!.id, event.position.dx, event.position.dy);
                      return result;
                    }

                    if (event.kind == PointerDeviceKind.touch) {
                      widget.inputManager!.sendTouch(GokaiTouchEvent(
                        type: GokaiTouchEventType.down,
                        windowId: snapshot.data!.id,
                        id: event.pointer,
                        x: event.position.dx,
                        y: event.position.dy,
                      ));
                    }
                    return GestureDisposition.rejected;
                  },
                  child: child,
                );
              }

              child = Focus(
                onFocusChange: snapshot.data!.setActive,
                child: child,
              );
            }

            if (!snapshot.data!.isToplevel) {
              child = Positioned.fromRect(
                rect: snapshot.data!.rect,
                child: child,
              );
            }

            if (widget.decorationBuilder != null && snapshot.data!.hasDecorations) {
              child = widget.decorationBuilder!(context, child, snapshot.data!);
            }
            return child;
          }
        }

        if (widget.progressBuilder != null) {
          return widget.progressBuilder!(context);
        }

        return const SizedBox();
      },
    );
}

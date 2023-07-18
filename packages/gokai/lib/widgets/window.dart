import 'package:event/event.dart';
import 'package:flutter/material.dart';
import 'package:gokai/view/window.dart';
import 'package:gokai/services.dart';

class GokaiWindowView extends StatefulWidget {
  const GokaiWindowView({
    super.key,
    required this.id,
    required this.windowManager,
    this.interactive = true,
    this.size,
    this.decorationBuilder,
    this.errorBuilder,
    this.progressBuilder,
  });

  final String id;
  final GokaiWindowManager windowManager;
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
                    ),
                  ).toList(),
                ],
              ),
            );

            if (widget.interactive) {
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

import 'package:flutter/material.dart';
import 'package:gokai/view/window.dart';
import 'package:gokai/services.dart';

class GokaiWindowView extends StatefulWidget {
  const GokaiWindowView({
    super.key,
    required this.id,
    required this.windowManager,
    this.errorBuilder,
    this.progressBuilder,
  });

  final String id;
  final GokaiWindowManager windowManager;
  final Widget Function(BuildContext context, Object error)? errorBuilder;
  final WidgetBuilder? progressBuilder;

  @override
  State<GokaiWindowView> createState() => _GokaiWindowViewState();
}

class _GokaiWindowViewState extends State<GokaiWindowView> {
  UniqueKey key = UniqueKey();
  GokaiWindow? window;
  Object? error;

  void _onCommit(String id) {
    if (id == widget.id) {
      setState(() {
        key = UniqueKey();
      });
    }
  }

  @override
  void initState() {
    super.initState();

    widget.windowManager.onCommit.add(_onCommit);
  }

  @override
  void dispose() {
    super.dispose();
    widget.windowManager.onCommit.remove(_onCommit);
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
            return Texture(
              textureId: snapshot.data!.texture!,
            );
          }
        }

        if (widget.progressBuilder != null) {
          return widget.progressBuilder!(context);
        }

        return const CircularProgressIndicator();
      },
    );
}
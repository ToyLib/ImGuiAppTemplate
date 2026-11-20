# Asset

このフォルダーの中身は、ビルドのたびに実行ファイルの隣の `Asset/` にコピーされます。

フォントを同梱するときは、ここに置いて `src/app/application.cpp` の `WindowOptions()` で `options.fontFile = "Asset/フォント名.ttf";` のように指定します。
指定しなければ、OS の日本語フォントを使います。

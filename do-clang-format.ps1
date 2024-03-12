Param([array] $files)

#
# clang-format.exeがあるかどうかを判定する。
#
if ( !( Get-Command clang-format.exe 2>$null ) ) {
    echo "clang-format not exists under PATH."
    Pause
    exit 1
}

#
# ファイル一覧を取得する。
#
if ($files.Count -eq 0) { # 引数なし？
    # フォルダから *.h, *.c のファイルを検索して対象とする。
    $files = Get-ChildItem -Path src -Recurse -Filter "*.h" -Name
    $files += Get-ChildItem -Path src -Recurse -Filter "*.c" -Name
}

#
# ファイル一覧に対して逐次実行
# note: $filesをどうやってリストとして扱っているのか不明。
#
for ( $i = 0; $i -lt $files.Count; $i++) {

    # ファイル名を取得
    $fileName = $files[$i]

    # 出力ファイル名を作成
    $outputFileName = $fileName + "~"

    echo "$fileName"

    # clang-formatを実行
    clang-format.exe $fileName > $outputFileName
    if ( $? ) {
        # 変更ファイル名を作成（Rename-Itemはフォルダを含んではいけない）
        $newName = Split-Path $fileName -Leaf

        # 前のファイルを削除
        Remove-Item $fileName

        # リネーム実行
        Rename-Item -Path $outputFileName -NewName $newName

    } else {
        # 失敗したので出力ファイルを削除
        Remove-Item $outputFileName
    }
}

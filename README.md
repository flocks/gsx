# GSX

## JSX-semantic grep

GSX uses tree-sitter API so you can grep for some JSX elements accross
your project.

## API

```console
$ gsx 'Button' // search all <Button> in your codebase
$ gsx 'Button.variant' // search all <Button> that have variant props
$ gsx 'Button.variant,size' // search all <Button> that have variant AND size props
$ gsx 'Button.variant,^size' // search all <Button> that have variant AND not size props
```

## Dependencies

GSX uses [ripgrep](https://github.com/BurntSushi/ripgrep) for an
initial filtering stage. This reduces the number of files requiring
AST construction.

The project embeds as git submodules 
- [tree-sitter lib](https://github.com/tree-sitter/tree-sitter)
- [tree-sitter-typescript grammar](https://github.com/tree-sitter/tree-sitter-typescript)


## Install

```console
git clone --recursive https://github.com/flocks/gsx
cd gsx
make
sudo make install
```

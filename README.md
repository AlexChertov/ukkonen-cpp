# ukkonen-cpp
A C++ implementation of the Ukkonen suffix tree algorithm

The implementation is flexible in the following ways:
- The user can specify the character type via the template parameter.
- The user can specify the kind of storage to use for children of a node via the second template parameter. The corresponding class must implement three methods
  - `size_t GetChid( charT c )` - returs the index of the child node which can be reached by following the given character. If there is no such child then `numeric_limits<size_t>::max()` is returned.
  - `SetChild(charT c, size_t iNode)` - sets a child node for a given character.
  - `vector<size_t> GetChildren()` - returns indices of all children. This method is not used for constructing the tree but it is useful when one wants to inspect the contents of one.

The package contains an implementation of child storage that uses hash-sets - `CharHashSetFinder`.

In a simple case, assuming the standard `char` type is used, a suffix tree can be created as follows:
```cpp
  const char* s = "BANANA";
  SuffixTree<char, CharHashSetFinder<char>> t(s, strlen(s));
```
The following sources helped me understand and implement the algorithm:

[ITMO Lecture Nodes (rus)](https://neerc.ifmo.ru/wiki/index.php?title=%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%A3%D0%BA%D0%BA%D0%BE%D0%BD%D0%B5%D0%BD%D0%B0)

[Explanation on Codeforces (rus)](http://codeforces.com/blog/entry/16780?locale=ru)

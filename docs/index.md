# Custom Features

## Bitwise Operators
Bitwise operators treat their operands (arguments) as binary but return as a lua number

#### Bitwise And
```lua
print(5 & 13);
```
Returns a `1` in each bit position for which the corresponding bits of both operands are `1`'s.

#### Bitwise Or
```lua
print(5 | 13);
```
Returns a `1` in each bit position for which the corresponding bits of either or both operands are `1`'s.

#### Bitwise Xor
```lua
print(5 ~ 13);
```
Returns a `1` in each bit position for which the corresponding bits of either but not both operands are `1`'s.

#### Bitwise Not
```lua
print(~ 5);
```
Inverts the bits of its operand.

#### Bitwise Left Shift
```lua
print(5 << 13);
```
Shifts the bits of `5` `13` left

#### Bitwise Right Shift
```lua
print(5 >> 13);
```
Shifts the bits of `5` `13` right

## Custom Functions

> setmarked(object obj, bool marked)
Sets whether the object is marked for garbage collection
> getmarked(object obj)
Gets whether the object is marked for garbage collection
> setcollectable(object obj, bool collectable)
Sets whether the object will considered for garbage collection

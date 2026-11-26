# LaTeX 数学符号

https://oeis.org/wiki/List_of_LaTeX_mathematical_symbols

## 符号

`\infty` $$ \infty $$

`\times` $$ \times $$

## 公式

`\cfrac` $$ \cfrac {a+1}{b+1} $$

`\sum` $$ \sum_{i=i}^{n} $$

`\prod` $$ \prod_{i=i}^{n} $$

## 关系运算符

`\equiv` $$ \equiv $$

`\doteq` $$ \doteq $$

`\approx` $$ \approx $$

`\cong` $$ \cong $$

`\simeq` $$ \simeq $$

`\sim` $$ \sim $$

`\nsim` $$ \nsim $$

`\propto` $$ \propto $$

`\neq \ne` $$ \neq \ne $$

## 箭头

`\leftarrow` $$ \leftarrow $$

`\longleftarrow` $$ \longleftarrow $$

`\uparrow` $$ \uparrow $$

`\Leftarrow` $$ \Leftarrow $$

`\Longleftarrow` $$ \Longleftarrow $$

`\Uparrow` $$ \Uparrow $$

`\rightarrow` $$ \rightarrow  $$

`\longrightarrow` $$ \longrightarrow $$

`\downarrow` $$ \downarrow $$

`\Rightarrow` $$ \Rightarrow $$

`\Longrightarrow` $$ \Longrightarrow $$

`\Downarrow` $$ \Downarrow $$

`\leftrightarrow` $$ \leftrightarrow $$

`\longleftrightarrow` $$ \longleftrightarrow $$

`\updownarrow` $$ \updownarrow $$

`\Leftrightarrow` $$ \Leftrightarrow $$

`\Longleftrightarrow` $$ \Longleftrightarrow $$

`\Updownarrow` $$ \Updownarrow $$

`\mapsto` $$ \mapsto $$

`\longmapsto` $$ \longmapsto $$

`\nearrow` $$ \nearrow  $$

`\hookleftarrow` $$ \hookleftarrow  $$

`\hookrightarrow` $$ \hookrightarrow $$

`\searrow` $$ \searrow $$

`\leftharpoonup` $$ \leftharpoonup  $$

`\rightharpoonup` $$ \rightharpoonup  $$

`\swarrow` $$ \swarrow $$

`\leftharpoondown` $$ \leftharpoondown  $$

`\rightharpoondown` $$ \rightharpoondown  $$

`\nwarrow` $$ \nwarrow  $$

`\rightleftharpoons` $$ \rightleftharpoons $$

`\leadsto` $$ \leadsto $$

## 空格

无空格： $$ a b $$

`\;` - 粗空格： $$ a \; b $$

`\:` - 中等空格： $$ a \: b $$

`\,` - 细空格： $$ a \, b $$

`\!` - 负细空格：  $$ a \! b $$

`\quad` - 等于当前字体大小的空格： $$ a \quad b $$

`\qquad` - 等于两个 `\quad`: $$ a \qquad b $$

`\hspace{length}` - 插入指定长度的水平空格：
$$ a \hspace{1cm} b $$
$$ a \hspace{10pt} b $$
$$ a \hspace{200px} b $$

`\\` - 换行， `\\[length]` - 换行并设置行间距：
$$ a \\[20pt] b $$

## 希腊字母

```latex
\alpha A        \beta B       \gamma \Gamma        \delta \Delta
\epsilon E      \zeta Z       \eta E               \theta \Theta
\iota I         \kappa K      \lambda \Lambda      \mu M
\nu N           \omicron O    \pi \Pi              \rho R
\sigma \Sigma   \tau T        \upsilon \Upsilon    \phi \Phi
\chi X          \psi \Psi     \omega \Omega
```

$$ \alpha A \;\;\;\; \beta B \;\;\;\; \gamma \Gamma \;\;\;\; \delta \Delta $$

$$ \epsilon E \;\;\;\; \zeta Z \;\;\;\; \eta E \;\;\;\; \theta \Theta $$

$$ \iota I \;\;\;\; \kappa K \;\;\;\; \lambda \Lambda \;\;\;\; \mu M $$

$$ \nu N \;\;\;\; \omicron O \;\;\;\; \pi \Pi \;\;\;\; \rho R $$

$$ \sigma \Sigma \;\;\;\; \tau T \;\;\;\; \upsilon \Upsilon \;\;\;\; \phi \Phi $$

$$ \chi X \;\;\;\; \psi \Psi \;\;\;\; \omega \Omega $$

## 矩阵

无括号：

```latex
\begin{matrix}
1 & 2 & 3\\
a & b & c
\end{matrix}
```

$$
\begin{matrix}
1 & 2 & 3\\
a & b & c
\end{matrix}
$$

圆括号：

```latex
\begin{pmatrix}
1 & 2 & 3\\
a & b & c
\end{pmatrix}
```

$$
\begin{pmatrix}
1 & 2 & 3\\
a & b & c
\end{pmatrix}
$$

方括号：

```latex
\begin{bmatrix}
1 & 2 & 3\\
a & b & c
\end{bmatrix}
```

$$
\begin{bmatrix}
1 & 2 & 3\\
a & b & c
\end{bmatrix}
$$

花括号：

```latex
\begin{Bmatrix}
1 & 2 & 3\\
a & b & c
\end{Bmatrix}
```

$$
\begin{Bmatrix}
1 & 2 & 3\\
a & b & c
\end{Bmatrix}
$$

竖线（单竖线）：

```latex
\begin{vmatrix}
1 & 2 & 3\\
a & b & c
\end{vmatrix}
```

$$
\begin{vmatrix}
1 & 2 & 3\\
a & b & c
\end{vmatrix}
$$

双竖线

```latex
\begin{Vmatrix}
1 & 2 & 3\\
a & b & c
\end{Vmatrix}
```

$$
\begin{Vmatrix}
1 & 2 & 3\\
a & b & c
\end{Vmatrix}
$$

## 括号

竖线

```latex
\bigg\rvert
```

$$
M\bigg\rvert_{n_2 < 0} = \begin{bmatrix}
                \cosh(\gamma L) & \cfrac {\sinh(\gamma L)} {\gamma} \\
                \gamma \: \sinh(\gamma L) & \cosh(\gamma L)
                \end{bmatrix}
$$

圆括号

```latex
(\, ... )\,
```

$$
(\, \cfrac {n_1}{n_2} )\,
\big(\, \cfrac {n_1}{n_2} \big)\,
\bigg(\, \cfrac {n_1}{n_2} \bigg)\,
$$

花括号

```latex
\{ ... \}
```

$$
\{ \cfrac {n_1}{n_2} \}
\big\{ \cfrac {n_1}{n_2} \big\}
\bigg\{ \cfrac {n_1}{n_2} \bigg\}
$$

方括号

```latex
[\, ... ]\,
```

$$
[\, \cfrac {n_1}{n_2} ]\,
\big[\, \cfrac {n_1}{n_2} \big]\,
\bigg[\, \cfrac {n_1}{n_2} \bigg]\,
$$


## 积分

```latex
\int_a^b
```

$$
\int_{-\infty}^{\infty}
$$

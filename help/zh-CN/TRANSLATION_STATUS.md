# reZonator 帮助文档汉化状态

原文位于 `help/` 目录，共 97 个 `.rst` 文件（约 3.17 万词）。
中文译文位于 `help/zh-CN/`，与原文同名，保留全部 RST 标记、directive、链接目标、公式、图片引用和代码，只翻译正文文字。

**状态：全部 97 个文件已译完（2026-11-20）。** 分五批完成：第一批 20 个（核心）、第二批 28 个（光路与元件操作）、第三批 20 个（函数与窗口）、第四批 18 个（参数/泵浦/自定义/设置）、第五批 11 个（Python/Lua API）。注：`help/matrix/` 子目录的单元件矩阵说明未纳入这 97 个统计，待后续单独评估。

## 术语统一

| 英文 | 中文 |
| --- | --- |
| laser resonator | 激光谐振腔 |
| Gaussian beam | 高斯光束 |
| beam waist | 束腰 |
| stability map | 稳定性图 |
| optical element | 光学元件 |
| schema | 光路 |
| round-trip | 往返 |
| caustic | 焦散 |
| tangential plane / sagittal plane | 切向平面 / 弧矢平面 |

说明：菜单路径（如 `Functions --> Caustic`）和界面按钮英文名称暂时保留英文，
因为软件界面本身尚未汉化，保留英文便于与实际界面对照。

## 第一批已译（20 个）

核心的目录、入门、光学系统类型、元件、主要功能类文件：

1. `index.rst` —— 文档首页 / 总目录
2. `intro.rst` —— 简介
3. `trip_type.rst` —— 往返类型（SW 驻波 / RR 环形谐振腔 / SP 单程）
4. `schema.rst` —— 光路
5. `elements.rst` —— 元件
6. `catalog.rst` —— 元件目录
7. `elem_library.rst` —— 自定义元件库
8. `layout.rst` —— 布局
9. `functions.rst` —— 函数（总览）
10. `func_caustic.rst` —— 焦散
11. `func_stabmap.rst` —— 稳定性图
12. `func_stabmap_2d.rst` —— 二维稳定性图
13. `func_beamdata.rst` —— 各元件处的光束参数
14. `func_reprate.rst` —— 重复频率
15. `input_beam.rst` —— 输入光束
16. `calc_gauss.rst` —— 高斯光束计算器
17. `gauss.rst` —— 高斯光束
18. `matrix_optics.rst` —— 谐振腔中光束参数的计算
19. `round_trip_matrix.rst` —— 往返矩阵的计算
20. `glossary.rst` —— 术语表

校验：已逐文件核对，译文中的图片引用数量、链接目标（`<...>`）和
`.. _label:` 标签与原文完全一致。

## 尚未翻译（77 个）

```
adjust.rst
app_settings.rst
app_settings_behavior.rst
app_settings_export.rst
app_settings_intf.rst
app_settings_lines.rst
calc_formula.rst
calc_grin.rst
calc_lens.rst
custom_func_lib.rst
custom_funcs.rst
custom_params.rst
custom_plot.rst
custom_script.rst
custom_table.rst
elem_matrs.rst
elem_opers.rst
elem_opers_add_adjuster.rst
elem_opers_append.rst
elem_opers_copy.rst
elem_opers_delete.rst
elem_opers_formula.rst
elem_opers_insert_into.rst
elem_opers_lock.rst
elem_opers_matrix.rst
elem_opers_merge.rst
elem_opers_move.rst
elem_opers_paste.rst
elem_opers_props.rst
elem_opers_replace.rst
elem_opers_save_custom.rst
elem_opers_slide.rst
elem_opers_split.rst
elem_props.rst
elem_props_options.rst
elem_props_outline.rst
elem_props_params.rst
func_beam_var.rst
func_caustic_mb.rst
func_caustic_mr.rst
func_freeze.rst
func_matrix.rst
func_matrix_mult.rst
func_misalign.rst
func_rt.rst
func_rt_plot.rst
func_windows.rst
handbook.rst
hyper_gauss.rst
info_window.rst
lua_primer.rst
memo_window.rst
misalign.rst
params_window.rst
plot_cursor.rst
plot_opers.rst
plot_window.rst
pump_mode.rst
pumps_window.rst
py_api.rst
py_element.rst
py_examples.rst
py_global.rst
py_matrix.rst
py_matrix3.rst
py_param_ref.rst
py_ray_vector.rst
py_round_trip.rst
py_schema.rst
schema_elems.rst
schema_window.rst
table_opers.rst
table_symbols.rst
table_window.rst
tools.rst
utils.rst
wavelen.rst
```

注：`help/matrix/` 子目录下的单个元件矩阵说明文件不在上述 97 个
`.rst` 统计范围内，尚未纳入任何一批，待主目录文件译完后再单独评估。

## 下一批建议顺序

- **第二批（光路与元件操作，约 20 个）**：`schema_window.rst`、
  `schema_elems.rst`、`elem_props.rst`、`elem_props_params.rst`、
  `elem_props_options.rst`、`elem_props_outline.rst`、`elem_matrs.rst`、
  `elem_opers.rst` 及其余 `elem_opers_*.rst` 子文件、`wavelen.rst`、
  `handbook.rst`、`utils.rst`、`tools.rst`。理由：这是第一批
  `schema.rst` / `elements.rst` 目录直接指向的下级页面。
- **第三批（其余函数与窗口，约 20 个）**：`func_windows.rst`、
  `func_caustic_mr.rst`、`func_caustic_mb.rst`、`func_beam_var.rst`、
  `func_matrix.rst`、`func_matrix_mult.rst`、`func_rt.rst`、
  `func_rt_plot.rst`、`func_freeze.rst`、`func_misalign.rst`、
  `plot_window.rst`、`plot_opers.rst`、`plot_cursor.rst`、
  `table_window.rst`、`table_opers.rst`、`table_symbols.rst`、
  `info_window.rst`、`memo_window.rst`、`adjust.rst`、
  `hyper_gauss.rst`。
- **第四批（参数、泵浦与自定义功能，约 19 个）**：`params_window.rst`、
  `pumps_window.rst`、`pump_mode.rst`、`custom_params.rst`、
  `custom_funcs.rst`、`custom_script.rst`、`custom_table.rst`、
  `custom_plot.rst`、`custom_func_lib.rst`、`calc_formula.rst`、
  `calc_grin.rst`、`calc_lens.rst`、`app_settings.rst` 及其余
  `app_settings_*.rst` 子文件、`misalign.rst`（篇幅最长，约 1500 词，单独留足时间）。
- **第五批（Python / Lua API，18 个）**：`py_api.rst`、`py_schema.rst`、
  `py_element.rst`、`py_examples.rst`、`py_global.rst`、
  `py_round_trip.rst`、`py_matrix.rst`、`py_matrix3.rst`、
  `py_param_ref.rst`、`py_ray_vector.rst`、`lua_primer.rst` 等剩余文件。
  理由：面向脚本用户，代码占比高，优先级最低。

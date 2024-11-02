# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# this file is looked for by cmake if you have --graphviz set

# in our particular case the libclang path is really long.
# a virtual target provides an indirection, and the exact
# path isn't required info per se.
set(GRAPHVIZ_IGNORE_TARGETS ".*libclang_rt.builtins.*")

# try to gussy up the graph a bit
set(GRAPHVIZ_GRAPH_HEADER \n\tgraph[splines="true",overlap="false"]\n)
#set(GRAPHVIZ_GRAPH_HEADER \tsize="5!" \n\tgraph[splines="true",forcelabels="true",overlap="false"] \n)
# set(GRAPHVIZ_GRAPH_HEADER \n\tgraph[splines="true",forcelabels="true",overlap="false"]\n\tnode7 [style="filled"]\n)

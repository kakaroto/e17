#! /bin/sh

xsltproc="java org.apache.xalan.xslt.Process -IN ../tools/filetypes.xml"
prefix=$1
treetempl="-XSL filetypes.xsl"
tabletempl="-XSL filetypes-to-tables.xsl"

echo "Generating trees in $1."
$xsltproc -OUT $prefix/code-a.html  $treetempl  -PARAM letter a
$xsltproc -OUT $prefix/table-a.html $tabletempl -PARAM letter a
echo -n "."
$xsltproc -OUT $prefix/code-b.html  $treetempl  -PARAM letter b
$xsltproc -OUT $prefix/table-b.html $tabletempl -PARAM letter b
echo -n "."
$xsltproc -OUT $prefix/code-c.html  $treetempl  -PARAM letter c
$xsltproc -OUT $prefix/table-c.html $tabletempl -PARAM letter c
echo -n "."
$xsltproc -OUT $prefix/code-d.html $treetempl   -PARAM letter d
$xsltproc -OUT $prefix/table-d.html $tabletempl -PARAM letter d
echo -n "."
$xsltproc -OUT $prefix/code-e.html $treetempl   -PARAM letter e
$xsltproc -OUT $prefix/table-e.html $tabletempl -PARAM letter e
echo -n "."
$xsltproc -OUT $prefix/code-f.html $treetempl   -PARAM letter f
$xsltproc -OUT $prefix/table-f.html $tabletempl -PARAM letter f
echo -n "."
$xsltproc -OUT $prefix/code-g.html $treetempl   -PARAM letter g
$xsltproc -OUT $prefix/table-g.html $tabletempl -PARAM letter g
echo -n "."
$xsltproc -OUT $prefix/code-h.html $treetempl   -PARAM letter h
$xsltproc -OUT $prefix/table-h.html $tabletempl -PARAM letter h
echo -n "."
$xsltproc -OUT $prefix/code-i.html $treetempl   -PARAM letter i
$xsltproc -OUT $prefix/table-i.html $tabletempl -PARAM letter i
echo -n "."
$xsltproc -OUT $prefix/code-j.html $treetempl   -PARAM letter j
$xsltproc -OUT $prefix/table-j.html $tabletempl -PARAM letter j
echo -n "."
$xsltproc -OUT $prefix/code-k.html $treetempl   -PARAM letter k
$xsltproc -OUT $prefix/table-k.html $tabletempl -PARAM letter k
echo -n "."
$xsltproc -OUT $prefix/code-l.html $treetempl   -PARAM letter l
$xsltproc -OUT $prefix/table-l.html $tabletempl -PARAM letter l
echo -n "."
$xsltproc -OUT $prefix/code-m.html $treetempl   -PARAM letter m
$xsltproc -OUT $prefix/table-m.html $tabletempl -PARAM letter m
echo -n "."
$xsltproc -OUT $prefix/code-n.html $treetempl   -PARAM letter n
$xsltproc -OUT $prefix/table-n.html $tabletempl -PARAM letter n
echo -n "."
$xsltproc -OUT $prefix/code-o.html $treetempl   -PARAM letter o
$xsltproc -OUT $prefix/table-o.html $tabletempl -PARAM letter o
echo -n "."
$xsltproc -OUT $prefix/code-p.html $treetempl   -PARAM letter p
$xsltproc -OUT $prefix/table-p.html $tabletempl -PARAM letter p
echo -n "."
$xsltproc -OUT $prefix/code-q.html $treetempl   -PARAM letter q
$xsltproc -OUT $prefix/table-q.html $tabletempl -PARAM letter q
echo -n "."
$xsltproc -OUT $prefix/code-r.html $treetempl   -PARAM letter r
$xsltproc -OUT $prefix/table-r.html $tabletempl -PARAM letter r
echo -n "."
$xsltproc -OUT $prefix/code-s.html $treetempl   -PARAM letter s
$xsltproc -OUT $prefix/table-s.html $tabletempl -PARAM letter s
echo -n "."
$xsltproc -OUT $prefix/code-t.html $treetempl   -PARAM letter t
$xsltproc -OUT $prefix/table-t.html $tabletempl -PARAM letter t
echo -n "."
$xsltproc -OUT $prefix/code-u.html $treetempl   -PARAM letter u
$xsltproc -OUT $prefix/table-u.html $tabletempl -PARAM letter u
echo -n "."
$xsltproc -OUT $prefix/code-v.html $treetempl   -PARAM letter v
$xsltproc -OUT $prefix/table-v.html $tabletempl -PARAM letter v
echo -n "."
$xsltproc -OUT $prefix/code-w.html $treetempl   -PARAM letter w
$xsltproc -OUT $prefix/table-w.html $tabletempl -PARAM letter w
echo -n "."
$xsltproc -OUT $prefix/code-x.html $treetempl   -PARAM letter x
$xsltproc -OUT $prefix/table-x.html $tabletempl -PARAM letter x
echo -n "."
$xsltproc -OUT $prefix/code-y.html $treetempl   -PARAM letter y
$xsltproc -OUT $prefix/table-y.html $tabletempl -PARAM letter y
echo -n "."
$xsltproc -OUT $prefix/code-z.html $treetempl   -PARAM letter z
$xsltproc -OUT $prefix/table-z.html $tabletempl -PARAM letter z
echo -n "."
$xsltproc -OUT $prefix/code-other.html $treetempl   -PARAM letter other
$xsltproc -OUT $prefix/table-other.html $tabletempl -PARAM letter other
echo "."

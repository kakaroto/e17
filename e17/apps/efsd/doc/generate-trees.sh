#! /bin/sh

xsltproc="java org.apache.xalan.xslt.Process -IN ../tools/filetypes.xml -XSL filetypes.xsl"
prefix=$1

echo "Generating trees in $1."
$xsltproc -OUT $prefix/code-a.html -PARAM letter a
echo -n "."
$xsltproc -OUT $prefix/code-b.html -PARAM letter b
echo -n "."
$xsltproc -OUT $prefix/code-c.html -PARAM letter c
echo -n "."
$xsltproc -OUT $prefix/code-d.html -PARAM letter d
echo -n "."
$xsltproc -OUT $prefix/code-e.html -PARAM letter e
echo -n "."
$xsltproc -OUT $prefix/code-f.html -PARAM letter f
echo -n "."
$xsltproc -OUT $prefix/code-g.html -PARAM letter g
echo -n "."
$xsltproc -OUT $prefix/code-h.html -PARAM letter h
echo -n "."
$xsltproc -OUT $prefix/code-i.html -PARAM letter i
echo -n "."
$xsltproc -OUT $prefix/code-j.html -PARAM letter j
echo -n "."
$xsltproc -OUT $prefix/code-k.html -PARAM letter k
echo -n "."
$xsltproc -OUT $prefix/code-l.html -PARAM letter l
echo -n "."
$xsltproc -OUT $prefix/code-m.html -PARAM letter m
echo -n "."
$xsltproc -OUT $prefix/code-n.html -PARAM letter n
echo -n "."
$xsltproc -OUT $prefix/code-o.html -PARAM letter o
echo -n "."
$xsltproc -OUT $prefix/code-p.html -PARAM letter p
echo -n "."
$xsltproc -OUT $prefix/code-q.html -PARAM letter q
echo -n "."
$xsltproc -OUT $prefix/code-r.html -PARAM letter r
echo -n "."
$xsltproc -OUT $prefix/code-s.html -PARAM letter s
echo -n "."
$xsltproc -OUT $prefix/code-t.html -PARAM letter t
echo -n "."
$xsltproc -OUT $prefix/code-u.html -PARAM letter u
echo -n "."
$xsltproc -OUT $prefix/code-v.html -PARAM letter v
echo -n "."
$xsltproc -OUT $prefix/code-w.html -PARAM letter w
echo -n "."
$xsltproc -OUT $prefix/code-x.html -PARAM letter x
echo -n "."
$xsltproc -OUT $prefix/code-y.html -PARAM letter y
echo -n "."
$xsltproc -OUT $prefix/code-z.html -PARAM letter z
echo -n "."
$xsltproc -OUT $prefix/code-other.html -PARAM letter other
echo "."

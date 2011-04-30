\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{efr}

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions

\LoadClass[11pt,a4paper]{article}
\RequirePackage{ifthen}
\RequirePackage{color}

\definecolor{linkcolor}{rgb}{0.21,0.18,0.28}
\definecolor{urlcolor}{rgb}{0.21,0.18,0.28}
\definecolor{citecolor}{rgb}{0.21,0.18,0.28}

\RequirePackage[pdfauthor={Efr},breaklinks=true,bookmarks=true,
  plainpages=true,colorlinks=true,urlcolor=urlcolor,
  linkcolor=linkcolor,citecolor=citecolor]{hyperref}

\usepackage{titlesec}
\usepackage{titletoc}
\usepackage{caption}
\usepackage[english]{babel}
\usepackage{fullpage} % small margins
\usepackage[xetex]{graphicx}
\usepackage{listings}
\usepackage{boxedminipage}
\usepackage{appendix}
\usepackage{fontspec}
\usepackage{xunicode}
\usepackage{xltxtra}
\defaultfontfeatures{Mapping=tex-text}

\renewcommand{\textbullet}{-}

\usepackage{fancyhdr}
\pagestyle{fancy}
% Header
\lhead{\linespread{0.7} \@title}
\chead{}
\rhead{\includegraphics[height=15pt]{logo_e.fr.png}}
\renewcommand{\headrulewidth}{0.5pt}
\renewcommand{\headheight}{45pt} % to fit 2 lines title in header + logo
\addtolength{\textheight}{-50pt}
\renewcommand{\headsep}{10pt}
\renewcommand{\footrulewidth}{0.5pt}

\newcommand{\Efr}{\textbf{Efr}}
\newcommand{\EfrMail}[1]{\def\TheEfrMail{#1}}

\renewcommand\maketitle{
  \begin{titlepage}
    \begin{center}
      \includegraphics[width=\textwidth]{logo_e.fr.png}
    \end{center}

    \vspace{5cm}

    \hrule
    \begin{center}
      {\huge\textsf{\textbf{\@title}}}

      \vspace{1.0cm}
      {
        \normalsize
        \@author \\
        \TheEfrMail
      }
    \end{center}
    \vspace{0.5cm}

    \begin{center}
      This work is licensed under Creative Commons Attribution-Share
      Alike 3.0 License
      (\url{http://creativecommons.org/licenses/by-sa/3.0/})
    \end{center}

    \begin{center}
      {\footnotesize\sffamily
        \@date
      }
    \end{center}
    \hrule

    \vspace{4.5cm}
  \end{titlepage}
}

\newenvironment{centeredboxedminipage}[1]{
\begin{center}
  \begin{boxedminipage}{#1}
    \begin{center}
}{
    \end{center}
  \end{boxedminipage}
\end{center}
}

\newenvironment{noticepage}[2][8cm]{
\thispagestyle{empty}
~
\\
\vspace{#1}
\begin{centeredboxedminipage}{#2}
}{
\end{centeredboxedminipage}
\pagebreak
}


% Listing (source code examples) setup
\lstset{
  frame=single,
  showstringspaces=false,
  extendedchars=true,
  language=C,
  backgroundcolor=\color[rgb]{0.95,0.95,0.95},
  rulecolor=\color[rgb]{0.3,0.3,0.3},
  basicstyle=\small\upshape\ttfamily,
  commentstyle=\color[rgb]{0.5,0.0,0.0}\rmfamily\itshape,
  keywordstyle=\color[rgb]{0.7,0.0,0.8}\bfseries,
  stringstyle=\color[rgb]{0.6,0.4,0.4},
  identifierstyle=\color[rgb]{0.2,0.2,0.9}
}

\newcommand{\sectionbreak}{\clearpage}

% Style of Table of Contents entries
\titlecontents{section}[1.5em]
              {\sffamily}
              % before with label:
              {\contentslabel{1.5em}}
              % before without label:
              {\hspace*{-1.5em}}
              {\titlerule*[1pc]{.}\contentspage}

\titlecontents{subsection}[2.5em]
              {\sffamily}
              % before with label:
              {\contentslabel{2.5em}}
              % before without label:
              {\hspace*{-2.5em}}
              {\titlerule*[1pc]{}\contentspage}

\titlecontents{subsubsection}[3.5em]
              {\sffamily}
              % before with label:
              {\contentslabel{3.5em}}
              % before without label:
              {\hspace*{-3.5em}}
              {\titlerule*[1pc]{}\contentspage}

% Captions styles
\captionsetup{font=small,labelfont={bf,sf},format=hang,justification=justified}

% Comments
\newcommand{\comment}[1]{{\bigskip\color{red}\hrule\large\texttt{\# #1}\hrule\bigskip}}

% Easy way to include graphics that should fill whole text width
\newcommand{\includegraphicsfullwidth}[1]{
  \begin{center}
    \includegraphics[width=\textwidth]{#1}
  \end{center}
}

% Easy way to define a floating figure environment, with label, caption
% and graphics to take the whole text width
\newcommand{\figurecomplete}[3][width=\textwidth]{
  \begin{figure}[htbp]
    \begin{center}
    \includegraphics[#1]{#2}
    \caption{#3}
    \label{fig:#2}
    \end{center}
  \end{figure}
}

% Reference figures using ``Figure #number'', parameter is the label,
% without ``fig:'' prefix.
\newcommand{\figureref}[1]{\figurename\nobreakspace\ref{fig:#1}}

% Utility to type and link to email address
\newcommand{\email}[1]{$<$\href{mailto:#1}{#1}$>$}

% Bibliography style
\bibliographystyle{alpha}

% Larger paragraph spacing
\setlength{\parskip}{10pt}
% No paragraph indent, already doing large spacing between them
\setlength{\parindent}{0pt}

\renewcommand{\emph}[1]{\textbf{#1}}


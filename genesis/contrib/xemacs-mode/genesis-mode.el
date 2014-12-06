;;; genesis-mode.el --- specific language support for Genesis Mode

;; Copyright (C) 2005-2006 Hugo Cornelis

;; Authors:    2005-2006 Hugo Cornelis
;; Maintainer: Hugo Cornelis

;; How to use :
;;
;; 1. Put the files in the genesis mode package somewhere in your
;; homedirectory, e.g. in the directory
;; "~/genesis/emacs/genesis-mode/".
;;
;; 2. Put in your emacs initialization file :
;; (load-file "~/genesis/emacs/genesis-mode/genesis-vars.el")
;; (load-file "~/genesis/emacs/genesis-mode/genesis-langs.el")
;; (load-file "~/genesis/emacs/genesis-mode/genesis-mode.el")
;; (load-file "~/genesis/emacs/genesis-mode/genesis-files.el")
;;

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to the
;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;(defconst c-version "5.20"
;  "CC Mode version number.")

;;; Commentary:

;; This package provides GNU Emacs major modes for editing Genesis
;; code.

;; NOTE: This mode does not perform font-locking (a.k.a syntactic
;; coloring, keyword highlighting, etc.) for any of the supported
;; modes.  Typically this is done by a package called font-lock.el
;; which I do *not* maintain.  You should contact the Emacs
;; maintainers for questions about coloring or highlighting in any
;; language mode.

;;; Code:

(eval-when-compile
  (require 'cc-defs))

;; sigh.  give in to the pressure, but make really sure all the
;; definitions we need are here
(if (or (not (fboundp 'functionp))
	(not (fboundp 'char-before))
	(not (c-safe (char-after) t))
	(not (fboundp 'when))
	(not (fboundp 'unless)))
    (require 'cc-mode-19))

(eval-when-compile
  (require 'cc-menus))

;(defvar c-buffer-is-cc-mode t
;  "Non-nil for all buffers with a `major-mode' derived from CC Mode.
;Otherwise, this variable is nil.  I.e. this variable is non-nil for
;`c-mode', `c++-mode', `objc-mode', `java-mode', `idl-mode', and any
;other non-CC Mode mode that calls `c-initialize-cc-mode'
;\(e.g. `awk-mode').")
;(make-variable-buffer-local 'c-buffer-is-cc-mode)
;(put 'c-buffer-is-cc-mode 'permanent-local t)

;(defvar c-initialize-on-load t
;  "When non-nil, CC Mode initializes when the cc-mode.el file is loaded.")
  


;; Other modes and packages which depend on CC Mode should do the
;; following to make sure everything is loaded and available for their
;; use:

(require 'cc-mode)
(c-initialize-cc-mode)


;;;###autoload
(defun genesis-mode ()
  "Major mode for editing genesis scripts.

The hook variable `genesis-mode-hook' is run with no args, if that
variable is bound and has a non-nil value.  Also the hook
`c-mode-common-hook' is run first.

Key bindings:
\\{genesis-mode-map}"
  (interactive)
  (c-initialize-cc-mode)
  (kill-all-local-variables)
  (set-syntax-table genesis-mode-syntax-table)
  (setq major-mode 'genesis-mode
	mode-name "genesis"
	local-abbrev-table genesis-mode-abbrev-table)
  (use-local-map genesis-mode-map)
  (c-common-init)
  (setq comment-start "// "
	comment-end ""
	c-conditional-key c-C++-conditional-key
	c-comment-start-regexp c-C++-comment-start-regexp
	c-class-key c-C++-class-key
	c-access-key c-C++-access-key
	c-recognize-knr-p nil
;;	imenu-generic-expression cc-imenu-c++-generic-expression
;;	imenu-case-fold-search nil
	)
  (run-hooks 'c-mode-common-hook)
  (run-hooks 'genesis-mode-hook)
  (c-update-modeline))

(defvar genesis-font-lock-keywords (purecopy
  (list
   (concat "\\<\\("
	   "function\\|if\\|else\\|elif\\|end\\|exit\\|quit\\|continue\\|break\\|"
	   "for\\(\\|each\\)\\|"
	   "return\\|while"
	   "\\)\\>")
;   ("#endif" "#else" "#ifdef" "#ifndef" "#if" "#include" "#define" "#undef")
   (cons (concat "\\<\\("
		 "int\\|str\\|float"
		 "\\)\\>")
	 'font-lock-type-face)
   (cons (concat "\\<\\("

		 ;; use listactions, listclasses, listobjects

		 "Ca_concen"
		 "\\|Kpores"
		 "\\|Mg_block"
		 "\\|Napores"
		 "\\|PID"
		 "\\|RC"
		 "\\|asc_file"
		 "\\|autocorr"
		 "\\|calculator"
		 "\\|compartment"
		 "\\|concchan"
		 "\\|concpool"
		 "\\|crosscorr"
		 "\\|ddsyn"
		 "\\|dif2buffer"
		 "\\|difbuffer"
		 "\\|diffamp"
		 "\\|difshell"
		 "\\|disk_in"
		 "\\|disk_out"
		 "\\|diskio"
		 "\\|efield"
		 "\\|enz"
		 "\\|event_tofile"
		 "\\|facsynchan"
		 "\\|fixbuffer"
		 "\\|freq_monitor"
		 "\\|funcgen"
		 "\\|fura2"
		 "\\|ghk"
		 "\\|hebbsynchan"
		 "\\|hh_channel"
		 "\\|hillpump"
		 "\\|hsolve"
		 "\\|interspike"
		 "\\|leakage"
		 "\\|metadata"
		 "\\|mmpump"
		 "\\|nernst"
		 "\\|neutral"
		 "\\|paramtableBF"
		 "\\|paramtableCG"
		 "\\|paramtableGA"
		 "\\|paramtableSA"
		 "\\|paramtableSS"
		 "\\|peristim"
		 "\\|playback"
		 "\\|pool"
		 "\\|pulsegen"
		 "\\|randomspike"
		 "\\|reac"
		 "\\|receptor"
		 "\\|receptor2"
		 "\\|script_out"
		 "\\|sigmoid"
		 "\\|spikegen"
		 "\\|spikehistory"
		 "\\|symcompartment"
		 "\\|synchan"
		 "\\|synchan2"
		 "\\|tab2Dchannel"
		 "\\|tabchannel"
		 "\\|tabcurrent"
		 "\\|tabgate"
		 "\\|table"
		 "\\|table2D"
		 "\\|taupump"
		 "\\|timetable"
		 "\\|variable"
		 "\\|vdep_channel"
		 "\\|xbutton"
		 "\\|xcell"
		 "\\|xcoredraw"
		 "\\|xdialog"
		 "\\|xdraw"
		 "\\|xdumbdraw"
		 "\\|xfastplot"
		 "\\|xform"
		 "\\|xgif"
		 "\\|xgraph"
		 "\\|ximage"
		 "\\|xlabel"
		 "\\|xpix"
		 "\\|xplot"
		 "\\|xshape"
		 "\\|xsphere"
		 "\\|xtext"
		 "\\|xtoggle"
		 "\\|xtree"
		 "\\|xvar"
		 "\\|xview"

;		 "neutral\\|channel\||compartment\\|hsolve\\|tabchannel"
;		 "\\|xaxis\\|xbutton\\|xcell\\|xcoredraw\\|xdialog\\|xdraw\\|xdumbdraw\\|xfastplot\\|xform\\|xgif\\|xgraph\\|ximage\\|xlabel\\|xpix\\|xplot\\|xshape\\|xsphere\\|xtext\\|xtree\\|xtoggle\\|xvar\\|xview"
		 "\\)\\>")
	 'font-lock-type-face)
   (cons (concat "\\<\\("

		 ;; use listcommands output instead

		 "abort"
		 "\\|abs"
		 "\\|acos"
		 "\\|addaction"
		 "\\|addalias"
		 "\\|addclass"
		 "\\|addescape"
		 "\\|addfield"
		 "\\|addforwmsg"
		 "\\|addglobal"
		 "\\|addmsg"
		 "\\|addmsgdef"
		 "\\|addobject"
		 "\\|addtask"
		 "\\|argc"
		 "\\|arglist"
		 "\\|argv"
		 "\\|asciidata"
		 "\\|asin"
		 "\\|atan"
		 "\\|call"
		 "\\|callfunc"
		 "\\|cd"
		 "\\|ce"
		 "\\|cellsheet"
		 "\\|check"
		 "\\|chr"
		 "\\|clearerrors"
		 "\\|closefile"
		 "\\|convert"
		 "\\|copy"
		 "\\|cos"
		 "\\|countchar"
		 "\\|countelementlist"
		 "\\|cpu"
		 "\\|create"
		 "\\|createmap"
		 "\\|debug"
		 "\\|delete"
		 "\\|deleteaction"
		 "\\|deleteall"
		 "\\|deleteclass"
		 "\\|deletefield"
		 "\\|deleteforwmsg"
		 "\\|deletemsg"
		 "\\|deletemsgdef"
		 "\\|deletetasks"
		 "\\|disable"
		 "\\|duplicatetable"
		 "\\|echo"
		 "\\|egg"
		 "\\|el"
		 "\\|enable"
		 "\\|enddump"
		 "\\|eof"
		 "\\|exists"
		 "\\|exit"
		 "\\|exp"
		 "\\|extern"
		 "\\|file2tab"
		 "\\|fileconnect"
		 "\\|findchar"
		 "\\|findsolvefield"
		 "\\|floatformat"
		 "\\|flushfile"
		 "\\|gaussian"
		 "\\|gen2spk"
		 "\\|genesis"
		 "\\|getarg"
		 "\\|getclock"
		 "\\|getdate"
		 "\\|getdefault"
		 "\\|getelementlist"
		 "\\|getenv"
		 "\\|getfield"
		 "\\|getfieldnames"
		 "\\|getglobal"
		 "\\|getmsg"
		 "\\|getparamGA"
		 "\\|getpath"
		 "\\|getsolvechildname"
		 "\\|getsolvecompname"
		 "\\|getstat"
		 "\\|getsyncount"
		 "\\|getsyndest"
		 "\\|getsynindex"
		 "\\|getsynsrc"
		 "\\|h"
		 "\\|help"
		 "\\|initdump"
		 "\\|initparamBF"
		 "\\|initparamCG"
		 "\\|initparamGA"
		 "\\|initparamSA"
		 "\\|initparamSS"
		 "\\|input"
		 "\\|isa"
		 "\\|le"
		 "\\|listcommands"
		 "\\|listescape"
		 "\\|listfiles"
		 "\\|listglobals"
		 "\\|listobjects"
		 "\\|loadtab"
		 "\\|log"
		 "\\|logfile"
		 "\\|max"
		 "\\|maxerrors"
		 "\\|maxwarnings"
		 "\\|min"
		 "\\|move"
		 "\\|msgsubstitute"
		 "\\|notes"
		 "\\|objsubstitute"
		 "\\|openfile"
		 "\\|planarconnect"
		 "\\|planardelay"
		 "\\|planardelay2"
		 "\\|planarweight"
		 "\\|planarweight2"
		 "\\|plane"
		 "\\|pope"
		 "\\|position"
		 "\\|pow"
		 "\\|printargs"
		 "\\|printenv"
		 "\\|pushe"
		 "\\|putevent"
		 "\\|pwe"
		 "\\|quit"
		 "\\|rand"
		 "\\|randcoord"
		 "\\|randseed"
		 "\\|readcell"
		 "\\|readfile"
		 "\\|reclaim"
		 "\\|relposition"
		 "\\|resched"
		 "\\|reset"
		 "\\|resetsynchanbuffers"
		 "\\|restore"
		 "\\|rotcoord"
		 "\\|round"
		 "\\|save"
		 "\\|scaletabchan"
		 "\\|setclock"
		 "\\|setdefault"
		 "\\|setenv"
		 "\\|setfield"
		 "\\|setfieldprot"
		 "\\|setglobal"
		 "\\|setmethod"
		 "\\|setparamGA"
		 "\\|setpostscript"
		 "\\|setprompt"
		 "\\|setrand"
		 "\\|setrandfield"
		 "\\|setsearch"
		 "\\|setupNaCa"
		 "\\|setupalpha"
		 "\\|setupgate"
		 "\\|setupghk"
		 "\\|setuptau"
		 "\\|sh"
		 "\\|shapematch"
		 "\\|showclocks"
		 "\\|showcommand"
		 "\\|showfield"
		 "\\|showmsg"
		 "\\|showobject"
		 "\\|showsched"
		 "\\|showstat"
		 "\\|silent"
		 "\\|simdump"
		 "\\|simobjdump"
		 "\\|simundump"
		 "\\|sin"
		 "\\|spkcmp"
		 "\\|sqrt"
		 "\\|stack"
		 "\\|step"
		 "\\|stop"
		 "\\|strcat"
		 "\\|strcmp"
		 "\\|strlen"
		 "\\|strncmp"
		 "\\|strsub"
		 "\\|substituteinfo"
		 "\\|substring"
		 "\\|swapdump"
		 "\\|syndelay"
		 "\\|tab2file"
		 "\\|tan"
		 "\\|trunc"
		 "\\|tweakalpha"
		 "\\|tweaktau"
		 "\\|useclock"
		 "\\|version"
		 "\\|volumeconnect"
		 "\\|volumedelay"
		 "\\|volumedelay2"
		 "\\|volumeweight"
		 "\\|volumeweight2"
		 "\\|where"
		 "\\|writecell"
		 "\\|writefile"
		 "\\|xcolorscale"
		 "\\|xgetstat"
		 "\\|xps"
		 "\\|xsimplot"

		 "\\|include"
		 "\\|pixflags\\|xflushevents\\|xhide\\|xinit\\|xlower\\|xmap\\|xpixflags\\|xraise\\|xshow\\|xshowontop\\|xtextload\\|xupdate"
		 "\\)\\>")
	 'font-lock-builtin-face)
   (cons (concat "#\\(define\\|e\\(lse\\|ndif\\)\\|"
		 "i\\(f\\(\\|def\\|ndef\\)\\|nclude\\)\\|undef\\)\\>")
	 'font-lock-keyword-face)
   '("^[ \n\t]*\\(function\\|extern\\)[ \t]+\\([^ \t]+\\)" 1 font-lock-function-name-face)
   '("\\(--- .* ---\\|=== .* ===\\)" . font-lock-string-face)
   ))
  "Additional expressions to highlight in Genesis mode.")

(put 'genesis-mode 'font-lock-defaults '(genesis-font-lock-keywords))

(provide 'genesis-mode)

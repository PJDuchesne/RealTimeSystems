/*
__/\\\\\\\\\\\\\_____/\\\\\\\\\\\__/\\\\\\\\\\\\____        
 _\/\\\/////////\\\__\/////\\\///__\/\\\////////\\\__       
  _\/\\\_______\/\\\______\/\\\_____\/\\\______\//\\\_      
   _\/\\\\\\\\\\\\\/_______\/\\\_____\/\\\_______\/\\\_     
    _\/\\\/////////_________\/\\\_____\/\\\_______\/\\\_    
     _\/\\\__________________\/\\\_____\/\\\_______\/\\\_   
      _\/\\\___________/\\\___\/\\\_____\/\\\_______/\\\__  
       _\/\\\__________\//\\\\\\\\\______\/\\\\\\\\\\\\/___
        _\///____________\/////////_______\////////////_____
-> Name:  ZooKeeper.cpp
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/ZooKeeper.h"

#include <string>

const std::string aardvark =
R"(
AARDVARK
               ,                                                        
         (`.  : \               __..----..__                            
          `.`.| |:          _,-':::''' '  `:`-._                        
            `.:\||       _,':::::'         `::::`-.                     
              \\`|    _,':::::::'     `:.     `':::`.                   
               ;` `-''  `::::::.                  `::\
            ,-'      .::'  `:::::.         `::..    `:\
          ,' /_) -.            `::.           `:.     |                 
        ,'.:     `    `:.        `:.     .::.          \
   __,-'   ___,..-''-.  `:.        `.   /::::.         |                
  |):'_,--'           `.    `::..       |::::::.      ::\
   `-'                 |`--.:_::::|_____\::::::::.__  ::|               
                       |   _/|::::|      \::::::|::/\  :|               
                       /:./  |:::/        \__:::):/  \  :\
      jrei           ,'::'  /:::|        ,'::::/_/    `. ``-.__         
                    ''''   (//|/\      ,';':,-'         `-.__  `'--..__ 
                                                             `''---::::'
)";

const std::string beaver =
R"(
BEAVER
             ___
           .="   "=._.---.
         ."         c ' Y'`p
        /   ,       `.  w_/
    jgs |   '-.   /     /
  _,..._|      )_-\ \_=.\
 `-....-'`------)))`=-'"`'"
)";

const std::string cow =
R"(
COW
                                       /;    ;\
                                   __  \\____//
                                  /{_\_/   `'\____
                                  \___   (o)  (o  }
       _____________________________/          :--'   
   ,-,'`@@@@@@@@       @@@@@@         \_    `__\
  ;:(  @@@@@@@@@        @@@             \___(o'o)
  :: )  @@@@          @@@@@@        ,'@@(  `===='  
  :: : @@@@@:          @@@@         `@@@:
  :: \  @@@@@:       @@@@@@@)    (  '@@@'
  ;; /\      /`,    @@@@@@@@@\   :@@@@@)        
  ::/  )    {_----------------:  :~`,~~;
 ;;'`; :   )                  :  / `; ;
;;;; : :   ;                  :  ;  ; :      -ba   
`'`' / :  :                   :  :  : :
    )_ \__;      ";"          :_ ;  \_\       `,','
    :__\  \    * `,'*         \  \  :  \   *  8`;'*  *
        `^'     \ :/           `^'  `-^-'   \v/ :  \/
)";

const std::string dog =
R"(
DOG
       __,-----._                       ,-.
     ,'   ,-.    \`---.          ,-----<._/
    (,.-. o:.`    )),"\\-._    ,'         `.
   ('"-` .\       \`:_ )\  `-;'-._          \
  ,,-.    \` ;  :  \( `-'     ) -._     :   `:
 (    \ `._\\ ` ;             ;    `    :    )
  \`.  `-.    __   ,         /  \        ;, (
   `.`-.___--'  `-          /    ;     | :   |
     `-' `-.`--._          '           ;     |
           (`--._`.                ;   /\    |
            \     '                \  ,  )   :
            |  `--::----            \'   ;  ;|
            \    .__,-      (        )   :  :|
             \    : `------; \      |    |   ;
              \   :       / , )     |    |  (
               \   \      `-^-|     |   / , ,\
                )  )  -hrr-   | -^- ;   `-^-^'
             _,' _ ;          |    |
            / , , ,'         /---. :
            `-^-^'          (  :  :,'
                             `-^--'
)";

const std::string elephant =
R"(
ELEPHANT
              ___.-~"~-._   __....__
            .'    `    \ ~"~        ``-.
           /` _      )  `\              `\
          /`  a)    /     |               `\
         :`        /      |                 \
    <`-._|`  .-.  (      /   .            `;\\
     `-. `--'_.'-.;\___/'   .      .       | \\
  _     /:--`     |        /     /        .'  \\
 ("\   /`/        |       '     '         /    :`;
 `\'\_/`/         .\     /`~`=-.:        /     ``
   `._.'          /`\    |      `\      /(
                 /  /\   |        `Y   /  \
          Jgs   J  /  Y  |         |  /`\  \
               /  |   |  |         |  |  |  |
              "---"  /___|        /___|  /__|
                     '"""         '"""  '"""
)";

const std::string frog =
R"(
FROG
             _____________________
             |###################|
             |###################|
             |###################|
             |###################|
 ((-----------------------------------------
 | \         /  /@@ \      /@@ \  \
  \ \,      /  (     )    (     )  \            _____
   \ \      |   \___/      \___/   |           /  __ \
    \ ""*-__/                      \           | |  | |
     ""*-_                         "-_         | |  """
          \    -.  _________   .-   __"-.__.-((  ))
           \,    \^    U    ^/     /  "-___--((  ))
             \,   \         /    /'            | |
              |    \       /   /'              | |
              |     "-----"    \               | |
             /                  "*-._          | |
            /   /\          /*-._    \         | |
           /   /  "\______/"     /   /         | |
          /   /                 /   /          | |
         /. ./                  |. .|          """
        /  | |                  / | \
       /   |  \                /  |  \
      /.-./.-.|               /.-.|.-.\
)";

const std::string gorilla =
R"(
GORILLA
                 _
             ,.-" "-.,
            /   ===   \
           /  =======  \
        __|  (o)   (0)  |__
       / _|    .---.    |_ \
      | /.----/ O O \----.\ |
       \/     |     |     \/
       |                   |
  -s   |                   |
       |                   |
       _\   -.,_____,.-   /_
   ,.-"  "-.,_________,.-"  "-.,
  /          |       |          \
 |           l.     .l           |
 |            |     |            |
 l.           |     |           .l
  |           l.   .l           | \,
  l.           |   |           .l   \,
   |           |   |           |      \,
   l.          |   |          .l        |
    |          |   |          |         |
    |          |---|          |         |
    |          |   |          |         |
    /"-.,__,.-"\   /"-.,__,.-"\"-.,_,.-"\
   |            \ /            |         |
   |             |             |         |
    \__|__|__|__/ \__|__|__|__/ \_|__|__/
)";

const std::string horse =
R"(
HORSE
                            _(\_/)
                          ,((((^`\
                         ((((  (6 \
                       ,((((( ,    \
   ,,,_              ,(((((  /"._  ,`,
  ((((\\ ,...       ,((((   /    `-.-'
  )))  ;'    `"'"'""((((   (
 (((  /            (((      \
  )) |                      |
 ((  |        .       '     |
 ))  \     _ '      `t   ,.')
 (   |   y;- -,-""'"-.\   \/
 )   / ./  ) /         `\  \
    |./   ( (           / /'
    ||     \\          //'|
    ||      \\       _//'||
    ||       ))     |_/  ||
    \_\     |_/          || -j
    `'"                  \_\
                         `'"
)";

// const std::string insect =
// R"(
// INSECT
//        _                           _
//       / `._                     _.' \
//      ( @ : `.                 .' : @ )
//       \  `.  `.  ._     _.  .'  .'  /
//        \;' `.  `.  \   /  .'  .' `;/
//         \`.  `.  \  \_/  /  .'  .'/
//          ) :-._`. \ (:) / .'_.-: (
//          (`.....,`.\/:\/.',.....')
//           >------._|:::|_.------<
//          / .'._>_.-|:::|-._<_.'. \
//          |o _.-'_.-^|:|^-._`-._ o|
//   jgs    |`'   ;_.-'|:|`-._;   `'|
//          ".o_.-' ;."|:|".; `-._o."   
//            ".__."   \:/   ".__."
//                      ^
// )";

// const std::string jester =
// R"(
// JESTER
//          (_) -
//                 '
//         @_  _    '
//          )\/(@    '
//        __(/ \--._
//       (,-.---'--'@
//        @ )0_0(     _
//          ('-')    (_)
//     '    _\Y/_
//     ' .-'-\-/-'-._  '
//     _ /    '*     \ '
//    (_)  /)  *    .-.))>'
//      ._/  \__*_ /\__'.
//  '<((_'    |__H/  \__\
//            /   ,_/ |_|
//            )-- /   |x|
//            \ _/    (_ x
//            /_/       \_\@
//           /_/
//  snd     /_/
//         /x/
//        (_ x
//          \_\@
// )";

const std::string kangaroo =
R"(
KANGAROO
               .
             /\ /l
            ((.Y(!
             \ |/
             /  6~6,
             \ _    +-.
              \`-=--^-'
               \ \
              _/  \
             (  .  Y
            /"\ `--^--v--.
           / _ `--"T~\/~\/
          / " ~\.  !
    _    Y      Y./'
   Y^|   |      |~~7
   | l   |     / ./'
   | `L  | Y .^/~T
   |  l  ! | |/| |
   | .`\/' | Y | !   RC
   l  "~   j l j_L______
    \,____{ __"~ __ ,\_,\_
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
)";

const std::string lizard =
R"(
LIZARD
      __ \/_
      (' \`\
   _\, \ \\/ 
    /`\/\ \\
         \ \\
          \ \\/\/_
          /\ \\'\
        __\ `\\\
         /|`  `\\
                \\
    jgs          \\
                  \\    ,
                   `---'  
)";

const std::string moose =
R"(
MOOSE
      _/\_       __/\__
      ) . (_    _) .' (
      `) '.(   ) .'  (`
       `-._\(_ )/__(~`
           (ovo)-.__.--._
           )             `-.______
  jgs/VK  /                       `---._
         ( ,// )                        \
          `''\/-.                        |
                 \                       | 
                 |                       |
)";

// const std::string ninja =
// R"(
// NINJA
//      /___\_/
//      |\_/|<\
//      (`o`) `   __(\_            |\_
//      \ ~ /_.-`` _|__)  ( ( ( ( /()/
//     _/`-`  _.-``               `\|
//  .-`      (    .-.           
// (   .-     \  /   `-._       
//  \  (\_    /\/        `-.__-()
//   `-|__)__/ /  /``-.   /_____8 
//         \__/  /     `-`      ` 
//        />|   /(    
// NDT   /| J   L \-. 
//       `` |   | 
//          L___J 
//           ( |   
//          .oO()  
// )";

const std::string octopus =
R"(
OCTOPUS
                        ___
                     .-'   `'.
                    /         \
                    |         ;
                    |         |           ___.--,
           _.._     |0) ~ (0) |    _.---'`__.-( (_.
    __.--'`_.. '.__.\    '--. \_.-' ,.--'`     `""`
   ( ,.--'`   ',__ /./;   ;, '.__.'`    __
   _`) )  .---.__.' / |   |\   \__..--""  """--.,_
  `---' .'.''-._.-'`_./  /\ '.  \ _.-~~~````~~~-._`-.__.'
        | |  .' _.-' |  |  \  \  '.               `~---`
         \ \/ .'     \  \   '. '-._)
          \/ /        \  \    `=.__`~-.
     jgs  / /\         `) )    / / `"".`\
    , _.-'.'\ \        / /    ( (     / /
     `--~`   ) )    .-'.'      '.'.  | (
            (/`    ( (`          ) )  '-;
             `      '-;         (-'
)";

const std::string parrot =
R"(
PARROT
 ______ __
   {-_-_= '. `'.
    {=_=_-  \   \
     {_-_   |   /
      '-.   |  /    .===,
   .--.__\  |_(_,==`  ( o)'-.
  `---.=_ `     ;      `/    \
      `,-_       ;    .'--') /
        {=_       ;=~`    `"`
         `//__,-=~`
         <<__ \\__
   jgs   /`)))/`)))
)";

// const std::string quarter =
// R"(
// QUARTER
//             _.-'~~`~~'-._
//          .'`  B   E   R  `'.
//         / I               T \
//       /`       .-'~"-.       `\
//      ; L      / `-    \      Y ;
//     ;        />  `.  -.|        ;
//     |       /_     '-.__)       |
//     |        |-  _.' \ |        |
//     ;        `~~;     \\        ;
//      ;  INGODWE /      \\)P    ;
//       \  TRUST '.___.-'`"     /
//        `\                   /`
//          '._   1 9 9 7   _.'
//      jgs    `'-..,,,..-'`
// )";

const std::string rabbit =
R"(
RABBIT
         ,
        /|      __
       / |   ,-~ /
      Y :|  //  /
      | jj /( .^
      >-"~"-v"
     /       Y
    jo  o    |
   ( ~T~     j
    >._-' _./
   /   "~"  |
  Y     _,  |
 /| ;-"~ _  l
/ l/ ,-"~    \
\//\/      .- \
 Y        /    Y   -R
 l       I     !
 ]\      _\    /"\
(" ~----( ~   Y.  )
~~~~~~~~~~~~~~~~~~~~~
)";

const std::string scorpion =
R"(
SCORPION
       ___ __ 
     _{___{__}\
    {_}      `\)            
   {_}        `            _.-''''--.._
   {_}                    //'.--.  \___`.
    { }__,_.--~~~-~~~-~~-::.---. `-.\  `.)
     `-.{_{_{_{_{_{_{_{_//  -- 8;=- `
        `-:,_.:,_:,_:,.`\\._ ..'=- , 
            // // // //`-.`\`   .-'/
   jgs     << << << <<    \ `--'  /----)
            ^  ^  ^  ^     `-.....--'''
)";

const std::string turtle =
R"(
TURTLE
                     .-.-.-._
                   .~\ /~\_/ \.
                 .~\_/~\_/ \_/~\.
               .~\_/ \_/ \_/ \_/~\
    .----.    /\_/ \_/ \_/ \_/ \_/\
   (o)(o)`\_ /_/ \_/ \_/ \_/ \_/ \_\
   |      , |/ \_/ \_/ \_/ \_/ \_/ \\.  _.;
    \ "  /  |\_/ \_/ \_/ \_/ \_/ \_/~\"'_.'
     `--'`--\/_\-/-\_/-\-/ \_/-\_/_\.-""
              /~/"""| |"""""| |'"\~\
    jgs     _/ /   _| |    _| |   \ \
           (___|  (___|   (___|  (___|
)";

const std::string unicorn =
R"(
UNICORN
               ,,))))))));,
            __)))))))))))))),
 \|/       -\(((((''''((((((((.
 -*-==//////((''  .     `)))))),
 /|\      ))| o    ;-.    '(((((                                  ,(,
          ( `|    /  )    ;))))'                               ,_))^;(~
             |   |   |   ,))((((_     _____------~~~-.        %,;(;(>';'~
             o_);   ;    )))(((` ~---~  `::           \      %%~~)(v;(`('~
                   ;    ''''````         `:       `:::|\,__,%%    );`'; ~
                  |   _                )     /      `:|`----'     `-'
            ______/\/~    |                 /        /
          /~;;.____/;;'  /          ___--,-(   `;;;/
         / //  _;______;'------~~~~~    /;;/\    /
        //  | |                        / ;   \;;,\
       (<_  | ;                      /',/-----'  _>
        \_| ||_                     //~;~~~~~~~~~
            `\_|                   (,~~  -TX
                                    \~\
                                     ~~
)";

const std::string vulture =
R"(
VULTURE
  .-'`\-,/^\ .-.         
 /    |  \  ( ee\   __   
|     |  |__/,--.`"`  `, 
|    /   .__/    `"""",/ 
|   /    /  |            
.'.-'    /__/ 
)";

const std::string wolf =
R"(
WOLF
*     *    *     /\__/\  *    ---    *
   *            /      \    /     \
        *   *  |  -  -  |  |       |*   
 *   __________| \     /|  |       |    
   /              \ T / |   \     /    
 /                      |  *  ---
|  ||     |    |       /             *
|  ||    /______\     / |*     *
|  | \  |  /     \   /  |
 \/   | |\ \      | | \ \       -M
      | | \ \     | |  \ \
      | |  \ \    | |   \ \
      '''   '''   '''    '''      
)";

// const std::string xwing =
// R"(
// XWING
//     .-.__        .-.  ___
//     |_|  '--.-.-(   \/\;;\_ .-._______.-.
//     (-)___     \ \ .-\ \;;\(   \       \ \
//      Y    '---._\_((Q)) \;;\\ .-\     __(_)
//      I           __'-' / .--.((Q))---'    \,
//      I     ___.-:    \|  |   \'-'_          \
//      A  .-'      \ .-.\   \   \ \ '--.__     '\
//      |  |____.----((Q))\   \__|--\_      \     '
//         ( )        '-'  \_  :  \-' '--.___\
//          Y                \  \  \       \(_)
//          I                 \  \  \         \,
//          I                  \  \  \          \
//          A                   \  \  \          '\
//          |              snd   \  \__|           '
//                                \_:.  \
//                                  \ \  \
//                                   \ \  \
//                                    \_\_|
// )";

// const std::string yoda =
// R"(
// YODA
//                    ____                  
//                 _.' :  `._               
//             .-.'`.  ;   .'`.-.           
//    __      / : ___\ ;  /___ ; \      __  
//  ,'_ ""--.:__;".-.";: :".-.":__;.--"" _`,
//  :' `.t""--.. '<@.`;_  ',@>` ..--""j.' `;
//       `:-.._J '-.-'L__ `-- ' L_..-;'     
//         "-.__ ;  .-"  "-.  : __.-"       
//             L ' /.------.\ ' J           
//    Bug       "-.   "--"   .-"            
//             __.l"-:_JL_;-";.__           
//          .-j/'.;  ;""""  / .'\"-.        
//        .' /:`. "-.:     .-" .';  `.      
//     .-"  / ;  "-. "-..-" .-"  :    "-.   
//  .+"-.  : :      "-.__.-"      ;-._   \
//  ; \  `.; ;                    : : "+. ; 
// )";

const std::string zebra =
R"(
ZEBRA
    \\/),
   ,'.' /,
  (_)- / /,
     /\_/ |__..--,  *
    (\ _ /\ \ \ / ).'
     \(-'./ / (_ //
      \\ \,'--'\_(
      )(_/  )_/ )_)
mrf  (_,'  (_.'(_.'
)";

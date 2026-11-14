(()=>{
const {Chess}=window.ChessLib;
const root=document.getElementById("play");
if(!root)return;
const TCS={"10+5":[600,5],"15+10":[900,10],"none":[0,0]};
const DEF={sound:true,legal:true,evalbar:false,wood:false,badges:true};
const SET=(()=>{try{return Object.assign({},DEF,JSON.parse(localStorage.getItem("cc_play")||"{}"))}catch(e){return {...DEF}}})();
const saveSet=()=>{try{localStorage.setItem("cc_play",JSON.stringify(SET))}catch(e){}};
const FILES="abcdefgh";
const STD="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const st={game:new Chess(),user:"w",colorPick:"w",tc:"none",elo:1000,flip:false,sel:null,view:0,over:null,thinking:false,clock:{w:0,b:0},running:false,anal:null,analyzing:false,drag:null,promo:null,gen:0,phase:"setup",times:[],turnStart:0,startFen:STD,mode:"play",fcls:{},fTok:0,vars:[],cv:-1,ck:0,arrows:[],marks:{},arDown:null,rcAt:0};

root.innerHTML=`<div class="pl-left"><div class="pl-wrap" id="plWrap"><div class="pl-eval" id="plEval"><div class="w"></div><div class="v"></div></div><div class="board pl-b" id="plB"></div><div class="pl-promo" id="plPromo"></div></div>
<div class="pl-tools"><button id="plGear" title="Settings"><svg viewBox="0 0 24 24"><path d="M19.4 13a7.5 7.5 0 0 0 0-2l2.1-1.6-2-3.5-2.5 1a7.6 7.6 0 0 0-1.7-1L15 3.3h-4l-.3 2.6a7.6 7.6 0 0 0-1.7 1l-2.5-1-2 3.5L6.6 11a7.5 7.5 0 0 0 0 2l-2.1 1.6 2 3.5 2.5-1a7.6 7.6 0 0 0 1.7 1l.3 2.6h4l.3-2.6a7.6 7.6 0 0 0 1.7-1l2.5 1 2-3.5zM12 15.5a3.5 3.5 0 1 1 0-7 3.5 3.5 0 0 1 0 7z"/></svg></button><button id="plFlip" title="Flip board"><svg viewBox="0 0 24 24"><path d="M7 3 3 7h3v9h2V7h3zM17 21l4-4h-3V8h-2v9h-3z"/></svg></button>
<div class="pl-pop" id="plPop"><label><span>Sound</span><input type="checkbox" id="plSound"></label><label><span>Show legal moves</span><input type="checkbox" id="plLegal"></label><label><span>Evaluation bar</span><input type="checkbox" id="plEvalOn"></label><label><span>Wood board</span><input type="checkbox" id="plWood"></label><label><span>Move icons on board</span><input type="checkbox" id="plBadges"></label></div></div></div>
<div class="pl-side" id="plSide">
<div id="plSetup" class="pl-sec">
<div class="pl-card"><div class="pl-lbl">Play as</div><div class="seg" id="plColor"><button data-v="w" class="on">White</button><button data-v="r">Random</button><button data-v="b">Black</button></div></div>
<div class="pl-card"><div class="pl-lbl">Time control</div><div class="seg" id="plTc"><button data-v="none" class="on">No clock</button><button data-v="10+5">10+5</button><button data-v="15+10">15+10</button></div></div>
<div class="pl-card"><div class="pl-lbl">Engine strength <span id="plElo">1000</span></div><input type="range" id="plStr" min="1000" max="3100" step="100" value="1000"><div class="pl-ticks"><span>1000</span><span class="pl-lw" id="plLw"></span><span>3100</span></div></div>
<button class="pl-btn main" id="plNew">Start game</button>
<div class="pl-card" id="plAnaCard"><div class="pl-lbl">Analyze</div><textarea class="pl-in" id="plImp" rows="3" spellcheck="false" placeholder="Paste PGN or FEN"></textarea><div class="pl-err" id="plImpErr"></div><button class="pl-btn" id="plImpGo">Analyze</button></div>
<div class="pl-card" id="plSaved"><div class="pl-lbl">Saved games</div><div class="pl-glist" id="plGList"></div></div>
</div>
<div id="plGame" class="pl-sec">
<div class="pl-clocks" id="plClocks"><div class="pl-clock" id="plClkTop"><span class="n"></span><span class="t"></span></div><div class="pl-clock" id="plClkBot"><span class="n"></span><span class="t"></span></div></div>
<div class="pl-top"><div class="pl-status" id="plStatus"></div><span class="pl-lvl" id="plLvl"></span><button class="pl-btn" id="plCopy">Copy PGN</button><button class="pl-btn main pl-again" id="plAgainTop">New game</button><div class="pl-rsw"><button class="pl-btn" id="plResign">Resign</button><div class="pl-rpop" id="plModal"><div class="pl-mt">Resign this game?</div><div class="pl-mrow"><button class="pl-btn" id="plMNo">Cancel</button><button class="pl-btn danger" id="plMYes">Resign</button></div></div></div></div>
<div class="pl-card pl-moves"><div class="pl-open" id="plOpen"></div><div class="pl-mv" id="plMoves"></div><div class="pl-nav"><button data-n="first" title="First"><svg viewBox="0 0 24 24"><path d="M18.4 6.4 17 5l-7 7 7 7 1.4-1.4L12.8 12zM11.4 6.4 10 5l-7 7 7 7 1.4-1.4L5.8 12z"/></svg></button><button data-n="prev" title="Previous"><svg viewBox="0 0 24 24"><path d="M15.4 6.4 14 5l-7 7 7 7 1.4-1.4L9.8 12z"/></svg></button><button data-n="next" title="Next"><svg viewBox="0 0 24 24"><path d="M8.6 17.6 10 19l7-7-7-7-1.4 1.4 5.6 5.6z"/></svg></button><button data-n="last" title="Last"><svg viewBox="0 0 24 24"><path d="M5.6 17.6 7 19l7-7-7-7-1.4 1.4 5.6 5.6zM12.6 17.6 14 19l7-7-7-7-1.4 1.4 5.6 5.6z"/></svg></button></div></div>
<div class="pl-btns"><button class="pl-btn" id="plAnalyze">Analyze</button><button class="pl-btn main pl-again" id="plAgain">New game</button></div>
<div class="pl-card pl-anal" id="plAnal"><svg id="plGraph" viewBox="0 0 300 70" preserveAspectRatio="none"></svg><div class="pl-tip" id="plTip"></div><div class="pl-best" id="plBest"></div><div class="pl-exw" id="plExW"></div><div class="pl-sum" id="plSum"></div></div>
</div>
</div>`;

const $=id=>document.getElementById(id);
const B=$("plB");

function makeEngine(path){
  const w=new Worker(path);let lis=[];
  w.onmessage=e=>{const l=typeof e.data==="string"?e.data:String(e.data);lis.slice().forEach(f=>f(l))};
  const send=c=>w.postMessage(c);
  const wait=(test)=>new Promise(res=>{const f=l=>{if(test(l)){lis=lis.filter(x=>x!==f);res(l)}};lis.push(f)});
  const ready=(async()=>{send("uci");await wait(l=>l==="uciok");send("isready");await wait(l=>l==="readyok")})();
  return {send,wait,ready,on:f=>{lis.push(f);return()=>{lis=lis.filter(x=>x!==f)}}};
}
let playEng=null,analEng=null;
const getPlay=()=>playEng||(playEng=makeEngine("lib/stockfish-19-lite-single.js"));
const getAnal=()=>analEng||(analEng=makeEngine("lib/stockfish-19-lite-single.js"));
let liveEng=null;const getLive=()=>liveEng||(liveEng=makeEngine("lib/stockfish-19-lite-single.js"));

const newG=()=>new Chess(st.startFen);
const sOff=()=>st.startFen.split(" ")[1]==="b"?1:0,sNum=()=>+st.startFen.split(" ")[5]||1;
const plyNo=i=>sNum()+Math.floor((i-1+sOff())/2);
const fenAt=i=>{const g=newG();const h=st.game.history({verbose:true});for(let k=0;k<i;k++)g.move(h[k].san);return g};
const hist=()=>st.game.history({verbose:true});
const live=()=>st.view===hist().length;
const canEx=()=>st.phase==="game"&&!st.analyzing&&(st.mode==="fen"||!!(st.anal&&st.anal.done));
function curState(){if(st.cv<0)return {g:live()?st.game:fenAt(st.view),last:st.view>0?hist()[st.view-1]:null};
  const v=st.vars[st.cv],g=fenAt(v.base);let last=null;for(let k=0;k<st.ck;k++)last=g.move(v.moves[k]);return {g,last}}
const act=()=>canEx()?curState().g:live()?st.game:null;
const resetVars=()=>{st.vars=[];st.cv=-1;st.ck=0;st.fcls={};st.fTok=(st.fTok||0)+1};
const userTurn=()=>st.phase==="game"&&!st.over&&(st.mode==="fen"||st.game.turn()===st.user)&&!st.thinking;
const mine=p=>!!p&&(canEx()?p.color===act().turn():p.color===st.user);

function sqName(dr,df){const r=st.flip?7-dr:dr,f=st.flip?7-df:df;return FILES[f]+(8-r)}
function render(){
  const EX=canEx(),CS=curState(),A=EX?CS.g:live()?st.game:null;
  const g=CS.g;
  const h=hist();const lm=CS.last;
  const legal=st.sel&&A&&SET.legal?A.moves({square:st.sel,verbose:true}):[];
  const dests=new Map(legal.map(m=>[m.to,m]));
  let chk=null;if(g.inCheck()){const bd=g.board();bd.forEach(row=>row.forEach(p=>{if(p&&p.type==="k"&&p.color===g.turn())chk=p.square}))}
  let html="";
  for(let dr=0;dr<8;dr++)for(let df=0;df<8;df++){
    const s=sqName(dr,df),p=g.get(s),f=FILES.indexOf(s[0]),r=8-+s[1];
    const cls=["sq",(r+f)%2?"d":"l"];
    if(lm&&(lm.from===s||lm.to===s))cls.push("last");
    if(st.sel===s&&A)cls.push("sel");
    if(chk===s)cls.push("chk");
    if(p&&A&&(EX?p.color===A.turn():userTurn()&&p.color===st.user))cls.push("mv");
    const d=dests.get(s);
    let lab="";if(df===0)lab+=`<span class="crd rk">${s[1]}</span>`;if(dr===7)lab+=`<span class="crd fl">${s[0]}</span>`;
    const pc=p?`<img class="pc${st.drag&&st.drag.from===s?" ghosted":""}" draggable="false" src="${PIECES[p.color==="w"?p.type.toUpperCase():p.type]}" alt="">`:"";
    const bc=SET.badges&&st.cv<0&&lm&&lm.to===s?(st.mode==="fen"?st.fcls[st.view]||null:st.anal&&st.anal.cls?st.anal.cls[st.view]:null):null;
    html+=`<div class="${cls.join(" ")}" data-sq="${s}">${st.marks[s]?`<span class="mkq ${st.marks[s]}"></span>`:""}${pc}${d?`<span class="${p?"cap":"dot"}"></span>`:""}${bc?`<span class="sqb">${badge(bc)}</span>`:""}${chk===s&&g.isCheckmate()?`<span class="sqb kmw"><span class="cb kmate"><svg viewBox="0 0 24 24"><g transform="rotate(180 12 12)" fill="#fff"><rect x="11" y="2.5" width="2" height="5.5" rx=".5"/><rect x="9" y="4" width="6" height="2" rx=".5"/><path d="M4.5 12.5C4.5 9.5 7.5 8.5 9.5 9.5L12 11l2.5-1.5c2-1 5 0 5 3 0 2-1.6 3.7-2.3 5H6.8c-.7-1.3-2.3-3-2.3-5Z"/><rect x="6" y="18.6" width="12" height="2.6" rx="1"/></g></svg></span></span>`:""}${lab}</div>`;
  }
  const lv=(st.mode==="fen"||st.cv>=0)&&LIVE[g.fen()],au=st.mode==="fen"||st.cv>=0?(lv&&lv.u):st.anal&&st.anal.done&&st.view>0&&st.anal.bestSan[st.view]&&st.anal.bestUci?st.anal.bestUci[st.view]:null;
  B.innerHTML=html+arrows([...(au?[au]:[]),...st.arrows]);
  if(st.anim){const an=st.anim,t=B.querySelector(`.sq[data-sq="${an.to}"] .pc`),fs=B.querySelector(`.sq[data-sq="${an.from}"]`),ts=B.querySelector(`.sq[data-sq="${an.to}"]`);if(t&&fs&&ts){const a=fs.getBoundingClientRect(),b=ts.getBoundingClientRect();t.style.transition="none";t.style.transform=`translate(${a.left-b.left}px,${a.top-b.top}px)`;t.style.zIndex=5;t.getBoundingClientRect();t.style.transition="transform .15s ease-out";t.style.transform=""}}st.anim=null;
  $("plWrap").classList.toggle("idle",st.phase==="setup");root.classList.toggle("setup",st.phase==="setup");$("plWrap").classList.toggle("side",st.cv>=0);  $("plSetup").style.display=st.phase==="setup"?"":"none";$("plGame").style.display=st.phase==="game"?"":"none";$("plClocks").style.display=st.tc==="none"||st.mode!=="play"?"none":"";
  renderEval();renderMoves();renderClocks();renderStatus();renderAnal();
}

function arrows(list){
  if(!list.length)return "";
  const xy=q=>{let f=FILES.indexOf(q[0]),r=8-+q[1];if(st.flip){f=7-f;r=7-r}return [f+.5,r+.5]};
  const one=u=>{const [x1,y1]=xy(u.slice(0,2)),[x2,y2]=xy(u.slice(2,4));const dx=x2-x1,dy=y2-y1;
    const pts=[[x1,y1]];if(Math.abs(dx)*Math.abs(dy)===2){Math.abs(dx)>Math.abs(dy)?pts.push([x2,y1]):pts.push([x1,y2])}pts.push([x2,y2]);
    const [ax,ay]=pts[pts.length-2],L=Math.hypot(x2-ax,y2-ay),ux=(x2-ax)/L,uy=(y2-ay)/L,px=-uy,py=ux;
    const [sx,sy]=pts[1],L0=Math.hypot(sx-x1,sy-y1);pts[0]=[x1+(sx-x1)/L0*.3,y1+(sy-y1)/L0*.3];
    const hl=.42,hh=.26,bx=x2-ux*hl,by=y2-uy*hl;pts[pts.length-1]=[bx+ux*.02,by+uy*.02];
    const f=v=>v.toFixed(3);
    return `<g><polyline points="${pts.map(p=>p.map(f).join(",")).join(" ")}"/><polygon points="${f(bx+px*hh)},${f(by+py*hh)} ${f(x2)},${f(y2)} ${f(bx-px*hh)},${f(by-py*hh)}"/></g>`};
  return `<svg class="pl-arrow" viewBox="0 0 8 8">${list.map(one).join("")}</svg>`;
}
function winp(cp){return 50+50*(2/(1+Math.exp(-0.00368208*cp))-1)}
function evalOf(i){return st.anal&&st.anal[i]}
const LIVE={};let liveBusy=false,liveWant=null;
async function liveEval(fen){liveWant=fen;if(liveBusy)return;liveBusy=true;const eng=getLive();await eng.ready;
  while(liveWant&&!(liveWant in LIVE)){const f=liveWant;const pos=new Chess(f);const sgn=pos.turn()==="w"?1:-1;
    if(pos.isCheckmate()){LIVE[f]={mate:0,cp:pos.turn()==="w"?-100000:100000};continue}if(pos.isDraw()){LIVE[f]={cp:0};continue}
    eng.send("position fen "+f);eng.send("go depth 22 movetime 1500");let last=null;const off=eng.on(l=>{if(l.startsWith("info")&&l.includes(" score ")&&!l.includes("bound"))last=l});const bl=await eng.wait(l=>l.startsWith("bestmove"));off();
    const m=last&&last.match(/score (cp|mate) (-?\d+)/);LIVE[f]=m?(m[1]==="mate"?{mate:+m[2]*sgn,cp:(+m[2]>0?1:-1)*sgn*100000}:{cp:+m[2]*sgn}):{cp:0};const bu=bl.split(" ")[1];if(bu&&bu!=="(none)")LIVE[f].u=bu}
  liveBusy=false;(st.mode==="fen"||st.cv>=0)?render():renderEval()}
function renderEval(){
  const wrap=$("plWrap");const show=!!st.anal||SET.evalbar||st.mode==="fen";
  wrap.classList.toggle("haseval",show);
  if(!show)return;
  let e;if(st.anal&&st.cv<0)e=evalOf(st.view);else{const f=curState().g.fen();e=LIVE[f];if(!e)liveEval(f)}
  const bar=$("plEval");bar.classList.toggle("flip",st.flip);if(!e)return;
  let pct=50,txt="";
  if(e){if(e.mate!==undefined){pct=e.mate>0?100:e.mate<0?0:(e.cp>0?100:0);txt=e.mate===0?"#":"M"+Math.abs(e.mate)}else{pct=winp(e.cp);txt=(Math.abs(e.cp)/100).toFixed(1)}}
  bar.querySelector(".w").style.height=pct+"%";
  const v=bar.querySelector(".v");v.textContent=txt;v.className="v "+(pct>=50?"bot":"top");
}

const BOOKI='<svg viewBox="0 0 18 19"><path opacity=".3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#D5A47D" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><path opacity=".3" d="M8.45,5.9c-1-.75-2.51-1.09-4.83-1.09H2.54v8.71H3.62a8.16,8.16,0,0,1,4.83,1.17Z"/><path opacity=".3" d="M9.54,14.69a8.14,8.14,0,0,1,4.84-1.17h1.08V4.81H14.38c-2.31,0-3.81.34-4.84,1.09Z"/><path fill="#fff" d="M8.45,5.4c-1-.75-2.51-1.09-4.83-1.09H3V13h.58a8.09,8.09,0,0,1,4.83,1.17Z"/><path fill="#fff" d="M9.54,14.19A8.14,8.14,0,0,1,14.38,13H15V4.31h-.58c-2.31,0-3.81.34-4.84,1.09Z"/></svg>';
const GOODI='<svg viewBox="0 0 18 19"><path opacity=".3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#95b776" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><path opacity=".2" d="M15.11,6.81,9.45,12.47,7.79,14.13a.39.39,0,0,1-.28.11.39.39,0,0,1-.27-.11L2.89,9.78a.39.39,0,0,1-.11-.28.39.39,0,0,1,.11-.27L4.28,7.85a.34.34,0,0,1,.12-.09l.15,0a.37.37,0,0,1,.15,0,.38.38,0,0,1,.13.09l2.69,2.68,5.650-5.65a.38.38,0,0,1,.13-.09.37.37,0,0,1,.15,0,.4.4,0,0,1,.15,0,.34.34,0,0,1,.12.09l1.39,1.38a.41.41,0,0,1,.08.13.33.33,0,0,1,0,.15.4.4,0,0,1,0,.15A.5.5,0,0,1,15.11,6.81Z"/><path fill="#fff" d="M15.11,6.31,9.45,12,7.790,13.63a.39.39,0,0,1-.28.11.39.39,0,0,1-.27-.11L2.89,9.28A.39.39,0,0,1,2.78,9a.39.39,0,0,1,.11-.27L4.28,7.35a.34.34,0,0,1,.12-.09l.15,0a.37.37,0,0,1,.15,0,.38.38,0,0,1,.13.09L7.52,10l5.65-5.65a.38.38,0,0,1,.13-.09.37.37,0,0,1,.15,0,.4.4,0,0,1,.15,0,.34.34,0,0,1,.12.09l1.39,1.38a.41.41,0,0,1,.08.13.33.33,0,0,1,0,.15.4.4,0,0,1,0,.15A.5.5,0,0,1,15.11,6.31Z"/></svg>';
const BESTI='<svg viewBox="0 0 18 19"><path opacity=".3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#81B64C" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><path opacity=".2" d="M9,3.43a.5.5,0,0,0-.27.08.46.46,0,0,0-.17.22L7.24,7.17l-3.68.19a.52.52,0,0,0-.26.1.53.53,0,0,0-.16.23.45.45,0,0,0,0,.28.44.44,0,0,0,.15.23l2.86,2.32-1,3.56a.45.45,0,0,0,0,.28.46.46,0,0,0,.17.22.41.41,0,0,0,.26.09.43.43,0,0,0,.27-.08l3.09-2,3.09,2a.46.46,0,0,0,.53,0,.46.46,0,0,0,.17-.22.53.53,0,0,0,0-.28l-1-3.56L14.71,8.2A.44.44,0,0,0,14.86,8a.45.45,0,0,0,0-.28.53.53,0,0,0-.16-.23.52.52,0,0,0-.26-.1l-3.68-.2L9.44,3.73a.46.46,0,0,0-.17-.22A.5.5,0,0,0,9,3.43Z"/><path fill="#fff" d="M9,2.93A.5.5,0,0,0,8.73,3a.46.46,0,0,0-.17.22L7.24,6.67l-3.68.19A.52.52,0,0,0,3.3,7a.53.53,0,0,0-.16.23.45.45,0,0,0,0,.28.44.44,0,0,0,.15.23L6.15,10l-1,3.56a.45.45,0,0,0,0,.28.46.46,0,0,0,.17.22.41.41,0,0,0,.26.09.43.43,0,0,0,.27-.08l3.09-2,3.09,2a.46.46,0,0,0,.53,0,.46.46,0,0,0,.17-.22.53.53,0,0,0,0-.28l-1-3.56L14.71,7.7a.44.44,0,0,0,.15-.23.45.45,0,0,0,0-.28A.53.53,0,0,0,14.7,7a.52.52,0,0,0-.26-.1l-3.68-.2L9.44,3.23A.46.46,0,0,0,9.27,3,.5.5,0,0,0,9,2.93Z"/></svg>';
const BLUNI='<svg viewBox="0 0 18 19"><g><path opacity="0.3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#FA412D" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><g opacity="0.2"><path d="M14.74,5.45A2.58,2.58,0,0,0,14,4.54,3.76,3.76,0,0,0,12.89,4a4.07,4.07,0,0,0-1.2-.19A3.92,3.92,0,0,0,10.51,4a5.87,5.87,0,0,0-.9.37,3,3,0,0,0-.32.2,3.46,3.46,0,0,1,.42.63,3.29,3.29,0,0,1,.36,1.47.31.31,0,0,0,.19-.06l.11-.08a2.9,2.9,0,0,1,.29-.19,3.89,3.89,0,0,1,.41-.17,1.55,1.55,0,0,1,.48-.07,1.1,1.1,0,0,1,.72.24.72.72,0,0,1,.23.26.8.8,0,0,1,.07.34,1,1,0,0,1-.25.67,7.71,7.71,0,0,1-.65.63,6.2,6.2,0,0,0-.48.43,2.93,2.93,0,0,0-.45.54,2.55,2.55,0,0,0-.33.66,2.62,2.62,0,0,0-.13.83V11a.24.24,0,0,0,0,.12.35.35,0,0,0,.17.17l.12,0h1.71l.12,0a.23.23,0,0,0,.1-.07.21.21,0,0,0,.06-.1.27.27,0,0,0,0-.12V10.8a1,1,0,0,1,.26-.7q.27-.28.66-.63A5.79,5.79,0,0,0,14.05,9a4.51,4.51,0,0,0,.48-.6,2.56,2.56,0,0,0,.36-.72,2.81,2.81,0,0,0,.14-1A2.66,2.66,0,0,0,14.74,5.45Z"/><path d="M12.38,12.65H10.5l-.12,0a.34.34,0,0,0-.18.29v1.82a.36.36,0,0,0,.08.23.23.23,0,0,0,.1.07l.12,0h1.88a.24.24,0,0,0,.12,0,.26.26,0,0,0,.11-.07.36.36,0,0,0,.07-.1.28.28,0,0,0,0-.13V13a.27.27,0,0,0,0-.12.61.61,0,0,0-.07-.1A.32.32,0,0,0,12.38,12.65Z"/><path d="M6.79,12.65H4.91l-.12,0a.34.34,0,0,0-.18.29v1.82a.36.36,0,0,0,.08.23.23.23,0,0,0,.1.07l.12,0H6.79a.24.24,0,0,0,.12,0A.26.26,0,0,0,7,15a.36.36,0,0,0,.07-.1.28.28,0,0,0,0-.13V13a.27.27,0,0,0,0-.12.61.61,0,0,0-.07-.1A.32.32,0,0,0,6.79,12.65Z"/><path d="M8.39,4.54A3.76,3.76,0,0,0,7.3,4a4.07,4.07,0,0,0-1.2-.19A3.92,3.92,0,0,0,4.92,4a5.87,5.87,0,0,0-.9.37,3.37,3.37,0,0,0-.55.38l-.21.19a.32.32,0,0,0,0,.41l1,1.2a.26.26,0,0,0,.2.12.48.48,0,0,0,.24-.06l.11-.08a2.9,2.9,0,0,1,.29-.19l.4-.17A1.66,1.66,0,0,1,6,6.06a1.1,1.1,0,0,1,.72.24.72.72,0,0,1,.23.26A.77.77,0,0,1,7,6.9a1,1,0,0,1-.26.67,7.6,7.6,0,0,1-.64.63,6.28,6.28,0,0,0-.49.43,2.93,2.93,0,0,0-.45.54,2.72,2.72,0,0,0-.33.66,2.62,2.62,0,0,0-.13.83V11a.43.43,0,0,0,0,.12.39.39,0,0,0,.08.1.18.18,0,0,0,.1.07.21.21,0,0,0,.12,0H6.72l.12,0a.23.23,0,0,0,.1-.07.36.36,0,0,0,.07-.1A.5.5,0,0,0,7,11V10.8a1,1,0,0,1,.27-.7A8,8,0,0,1,8,9.47c.18-.15.35-.31.52-.48A7,7,0,0,0,9,8.39a3.23,3.23,0,0,0,.36-.72,3.07,3.07,0,0,0,.13-1,2.66,2.66,0,0,0-.29-1.27A2.58,2.58,0,0,0,8.39,4.54Z"/></g><g><path fill="#fff" d="M14.74,5A2.58,2.58,0,0,0,14,4a3.76,3.76,0,0,0-1.09-.56,4.07,4.07,0,0,0-1.2-.19,3.92,3.92,0,0,0-1.18.17,5.87,5.87,0,0,0-.9.37,3,3,0,0,0-.32.2,3.46,3.46,0,0,1,.42.63,3.29,3.29,0,0,1,.36,1.47.31.31,0,0,0,.19-.06L10.37,6a2.9,2.9,0,0,1,.29-.19,3.89,3.89,0,0,1,.41-.17,1.55,1.55,0,0,1,.48-.07,1.1,1.1,0,0,1,.72.24.72.72,0,0,1,.23.26.8.8,0,0,1,.07.34,1,1,0,0,1-.25.67,7.71,7.71,0,0,1-.65.63,6.2,6.2,0,0,0-.48.43,2.93,2.93,0,0,0-.45.54,2.55,2.55,0,0,0-.33.66,2.62,2.62,0,0,0-.13.83v.35a.24.24,0,0,0,0,.12.35.35,0,0,0,.17.17l.12,0h1.71l.12,0a.23.23,0,0,0,.1-.07.21.21,0,0,0,.06-.1.27.27,0,0,0,0-.12V10.3a1,1,0,0,1,.26-.7q.27-.28.66-.63a5.79,5.79,0,0,0,.51-.48,4.51,4.51,0,0,0,.48-.6,2.56,2.56,0,0,0,.36-.72,2.81,2.81,0,0,0,.14-1A2.66,2.66,0,0,0,14.74,5Z"/><path fill="#fff" d="M12.38,12.15H10.5l-.12,0a.34.34,0,0,0-.18.29v1.82a.36.36,0,0,0,.08.23.23.23,0,0,0,.1.07l.12,0h1.88a.24.24,0,0,0,.12,0,.26.26,0,0,0,.11-.07.36.36,0,0,0,.07-.1.28.28,0,0,0,0-.13V12.46a.27.27,0,0,0,0-.12.61.61,0,0,0-.07-.1A.32.32,0,0,0,12.38,12.15Z"/><path fill="#fff" d="M6.79,12.15H4.91l-.12,0a.34.34,0,0,0-.18.29v1.82a.36.36,0,0,0,.08.23.23.23,0,0,0,.1.07l.12,0H6.79a.24.24,0,0,0,.12,0A.26.26,0,0,0,7,14.51a.36.36,0,0,0,.07-.1.28.28,0,0,0,0-.13V12.46a.27.27,0,0,0,0-.12.61.61,0,0,0-.07-.1A.32.32,0,0,0,6.79,12.15Z"/><path fill="#fff" d="M8.39,4A3.76,3.76,0,0,0,7.3,3.48a4.07,4.07,0,0,0-1.2-.19,3.92,3.92,0,0,0-1.18.17,5.87,5.87,0,0,0-.9.37,3.37,3.37,0,0,0-.55.38l-.21.19a.32.32,0,0,0,0,.41l1,1.2a.26.26,0,0,0,.2.12.48.48,0,0,0,.24-.06L4.78,6a2.9,2.9,0,0,1,.29-.19l.4-.17A1.66,1.66,0,0,1,6,5.56a1.1,1.1,0,0,1,.72.24.72.72,0,0,1,.23.26A.77.77,0,0,1,7,6.4a1,1,0,0,1-.26.67,7.6,7.6,0,0,1-.64.63,6.28,6.28,0,0,0-.49.43,2.93,2.93,0,0,0-.45.54,2.72,2.72,0,0,0-.33.66,2.62,2.62,0,0,0-.13.83v.35a.43.43,0,0,0,0,.12.39.39,0,0,0,.08.1.18.18,0,0,0,.1.07.21.21,0,0,0,.12,0H6.72l.12,0a.23.23,0,0,0,.1-.07.36.36,0,0,0,.07-.1.5.5,0,0,0,0-.12V10.3a1,1,0,0,1,.27-.7A8,8,0,0,1,8,9c.18-.15.35-.31.52-.48A7,7,0,0,0,9,7.89a3.23,3.23,0,0,0,.36-.72,3.07,3.07,0,0,0,.13-1A2.66,2.66,0,0,0,9.15,5,2.58,2.58,0,0,0,8.39,4Z"/></g></g></svg>';
const INACI='<svg viewBox="0 0 18 19"><g><path opacity="0.3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#F7C631" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><g opacity="0.2"><path d="M13.66,14.8a.28.28,0,0,1,0,.13.23.23,0,0,1-.08.11.28.28,0,0,1-.11.08l-.12,0h-2l-.13,0a.27.27,0,0,1-.1-.08A.36.36,0,0,1,11,14.8V12.9a.59.59,0,0,1,0-.13.36.36,0,0,1,.07-.1l.1-.08.13,0h2a.33.33,0,0,1,.23.1.39.39,0,0,1,.08.1.28.28,0,0,1,0,.13Zm-.12-3.93a.31.31,0,0,1,0,.13.3.3,0,0,1-.07.1.3.3,0,0,1-.23.08H11.43a.31.31,0,0,1-.34-.31L10.94,4.1A.5.5,0,0,1,11,3.86l.11-.08.13,0h2.11a.35.35,0,0,1,.26.1.41.41,0,0,1,.08.24Z"/><path d="M7.65,14.82a.27.27,0,0,1,0,.12.26.26,0,0,1-.07.11l-.1.07-.13,0H5.43a.25.25,0,0,1-.12,0,.27.27,0,0,1-.1-.08.31.31,0,0,1-.09-.22V13a.36.36,0,0,1,.09-.23l.1-.07.12,0H7.32a.32.32,0,0,1,.23.09.3.3,0,0,1,.07.1.28.28,0,0,1,0,.13Zm2.2-7.17a3.1,3.1,0,0,1-.36.73A5.58,5.58,0,0,1,9,9a4.85,4.85,0,0,1-.52.49,8,8,0,0,0-.65.63,1,1,0,0,0-.27.7V11a.21.21,0,0,1,0,.12.17.17,0,0,1-.06.1.23.23,0,0,1-.1.07l-.12,0H5.53a.21.21,0,0,1-.12,0,.18.18,0,0,1-.1-.07.2.2,0,0,1-.08-.1.37.37,0,0,1,0-.12v-.35a2.68,2.68,0,0,1,.13-.84,2.91,2.91,0,0,1,.33-.66,3.38,3.38,0,0,1,.45-.55c.16-.15.33-.29.49-.42a7.84,7.84,0,0,0,.65-.64,1,1,0,0,0,.25-.67.77.77,0,0,0-.07-.34.67.67,0,0,0-.23-.27A1.16,1.16,0,0,0,6.49,6,1.61,1.61,0,0,0,6,6.11a3,3,0,0,0-.41.18,1.75,1.75,0,0,0-.29.18l-.11.09A.5.5,0,0,1,5,6.62a.31.31,0,0,1-.21-.13l-1-1.21a.3.3,0,0,1,0-.4A1.36,1.36,0,0,1,4,4.68a3.07,3.07,0,0,1,.56-.38,5.49,5.49,0,0,1,.9-.37,3.69,3.69,0,0,1,1.19-.17,3.92,3.92,0,0,1,2.3.75,2.85,2.85,0,0,1,.77.92A2.82,2.82,0,0,1,10,6.71,3,3,0,0,1,9.85,7.65Z"/></g><g><path fill="#fff" d="M13.66,14.3a.28.28,0,0,1,0,.13.23.23,0,0,1-.08.11.28.28,0,0,1-.11.08l-.12,0h-2l-.13,0a.27.27,0,0,1-.1-.08A.36.36,0,0,1,11,14.3V12.4a.59.59,0,0,1,0-.13.36.36,0,0,1,.07-.1l.1-.08.13,0h2a.33.33,0,0,1,.23.1.39.39,0,0,1,.08.1.28.28,0,0,1,0,.13Zm-.12-3.93a.31.31,0,0,1,0,.13.3.3,0,0,1-.07.1.3.3,0,0,1-.23.08H11.43a.31.31,0,0,1-.34-.31L10.94,3.6A.5.5,0,0,1,11,3.36l.11-.08.13,0h2.11a.35.35,0,0,1,.26.1.41.41,0,0,1,.08.24Z"/><path fill="#fff" d="M7.65,14.32a.27.27,0,0,1,0,.12.26.26,0,0,1-.07.11l-.1.07-.13,0H5.43a.25.25,0,0,1-.12,0,.27.27,0,0,1-.1-.08.31.31,0,0,1-.09-.22V12.49a.36.36,0,0,1,.09-.23l.1-.07.12,0H7.32a.32.32,0,0,1,.23.09.3.3,0,0,1,.07.1.28.28,0,0,1,0,.13Zm2.2-7.17a3.1,3.1,0,0,1-.36.73,5.58,5.58,0,0,1-.49.6A4.85,4.85,0,0,1,8.48,9a8,8,0,0,0-.65.63,1,1,0,0,0-.27.7v.22a.21.21,0,0,1,0,.12.17.17,0,0,1-.06.1.23.23,0,0,1-.1.07l-.12,0H5.53a.21.21,0,0,1-.12,0,.18.18,0,0,1-.1-.07.2.2,0,0,1-.08-.1.37.37,0,0,1,0-.12v-.35a2.68,2.68,0,0,1,.13-.84,2.91,2.91,0,0,1,.33-.66,3.38,3.38,0,0,1,.45-.55c.16-.15.33-.29.49-.42a7.84,7.84,0,0,0,.65-.64,1,1,0,0,0,.25-.67.77.77,0,0,0-.07-.34.67.67,0,0,0-.23-.27,1.16,1.16,0,0,0-.72-.24A1.61,1.61,0,0,0,6,5.61a3,3,0,0,0-.41.18A1.75,1.75,0,0,0,5.3,6l-.11.09A.5.5,0,0,1,5,6.12.31.31,0,0,1,4.74,6l-1-1.21a.3.3,0,0,1,0-.4A1.36,1.36,0,0,1,4,4.18a3.07,3.07,0,0,1,.56-.38,5.49,5.49,0,0,1,.9-.37,3.69,3.69,0,0,1,1.19-.17A3.92,3.92,0,0,1,8.93,4a2.85,2.85,0,0,1,.77.92A2.82,2.82,0,0,1,10,6.21,3,3,0,0,1,9.85,7.15Z"/></g></g></svg>';
const MISSI='<svg viewBox="0 0 18 19"><g><path opacity=".3" d="M9,.5C4.03,.5,0,4.53,0,9.5s4.03,9,9,9,9-4.03,9-9S13.97,.5,9,.5Z"/><path fill="#FF7769" d="M9,0C4.03,0,0,4.03,0,9s4.03,9,9,9,9-4.03,9-9S13.97,0,9,0Z"/><g opacity=".2"><path d="M13.99,12.51s.06,.08,.08,.13c.02,.05,.03,.1,.03,.15s-.01,.1-.03,.15c-.02,.05-.05,.09-.08,.13l-1.37,1.37s-.08,.06-.13,.08c-.05,.02-.1,.03-.15,.03s-.1-.01-.15-.03c-.05-.02-.09-.05-.13-.08l-3.06-3.06-3.06,3.06s-.08,.06-.13,.08c-.05,.02-.1,.03-.15,.03s-.1-.01-.15-.03c-.05-.02-.09-.05-.13-.08l-1.37-1.37c-.07-.07-.11-.17-.11-.28s.04-.2,.11-.28l3.06-3.06-3.06-3.06c-.07-.07-.11-.17-.11-.28s.04-.2,.11-.28l1.37-1.37c.07-.07,.17-.11,.28-.11s.2,.04,.28,.11l3.06,3.06,3.06-3.06c.07-.07,.17-.11,.28-.11s.2,.04,.28,.11l1.37,1.37s.06,.08,.08,.13c.02,.05,.03,.1,.03,.15s-.01,.1-.03,.15c-.02,.05-.05,.09-.08,.13l-3.06,3.06,3.06,3.06Z"/></g><path fill="#f1f2f2" d="M13.99,12.01s.06,.08,.08,.13c.02,.05,.03,.1,.03,.15s-.01,.1-.03,.15c-.02,.05-.05,.09-.08,.13l-1.37,1.37s-.08,.06-.13,.08c-.05,.02-.1,.03-.15,.03s-.1-.01-.15-.03c-.05-.02-.09-.05-.13-.08l-3.06-3.06-3.06,3.06s-.08,.06-.13,.08c-.05,.02-.1,.03-.15,.03s-.1-.01-.15-.03c-.05-.02-.09-.05-.13-.08l-1.37-1.37c-.07-.07-.11-.17-.11-.28s.04-.2,.11-.28l3.06-3.06-3.06-3.06c-.07-.07-.11-.17-.11-.28s.04-.2,.11-.28l1.37-1.37c.07-.07,.17-.11,.28-.11s.2,.04,.28,.11l3.06,3.06,3.06-3.06c.07-.07,.17-.11,.28-.11s.2,.04,.28,.11l1.37,1.37s.06,.08,.08,.13c.02,.05,.03,.1,.03,.15s-.01,.1-.03,.15c-.02,.05-.05,.09-.08,.13l-3.06,3.06,3.06,3.06Z"/></g></svg>';
const BRILI='<svg viewBox="0 0 18 19"><path opacity=".3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#26c2a3" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><g opacity=".2"><path d="M12.57,14.6a.51.51,0,0,1,0,.13.44.44,0,0,1-.08.11l-.11.08-.13,0h-2l-.13,0L10,14.84A.41.41,0,0,1,10,14.6V12.7a.32.32,0,0,1,.09-.23.39.39,0,0,1,.1-.08l.13,0h2a.31.31,0,0,1,.24.1.39.39,0,0,1,.08.1.51.51,0,0,1,0,.13Zm-.12-3.93a.17.17,0,0,1,0,.12.41.41,0,0,1-.07.11.4.4,0,0,1-.23.08H10.35a.31.31,0,0,1-.34-.31L9.86,3.9A.36.36,0,0,1,10,3.66a.23.23,0,0,1,.11-.08.27.27,0,0,1,.13,0H12.3a.32.32,0,0,1,.25.1.36.36,0,0,1,.09.24Z"/><path d="M8.07,14.6a.51.51,0,0,1,0,.13.44.44,0,0,1-.08.11l-.11.08-.13,0h-2l-.13,0-.11-.08a.41.41,0,0,1-.08-.24V12.7a.27.27,0,0,1,0-.13.36.36,0,0,1,.07-.1.39.39,0,0,1,.1-.08l.13,0h2a.31.31,0,0,1,.24.1.39.39,0,0,1,.08.1.51.51,0,0,1,0,.13ZM8,10.67a.17.17,0,0,1,0,.12.41.41,0,0,1-.07.11.4.4,0,0,1-.23.08H5.85a.31.31,0,0,1-.34-.31L5.36,3.9a.36.36,0,0,1,.09-.24.23.23,0,0,1,.11-.08.27.27,0,0,1,.13,0H7.8a.35.35,0,0,1,.25.1.36.36,0,0,1,.09.24Z"/></g><path fill="#fff" d="M12.57,14.1a.51.51,0,0,1,0,.13.44.44,0,0,1-.08.11l-.11.08-.13,0h-2l-.13,0L10,14.34A.41.41,0,0,1,10,14.1V12.2A.32.32,0,0,1,10,12a.39.39,0,0,1,.1-.08l.13,0h2a.31.31,0,0,1,.24.1.39.39,0,0,1,.08.1.51.51,0,0,1,0,.13Zm-.12-3.93a.17.17,0,0,1,0,.12.41.41,0,0,1-.07.11.4.4,0,0,1-.23.08H10.35a.31.31,0,0,1-.34-.31L9.86,3.4A.36.36,0,0,1,10,3.16a.23.23,0,0,1,.11-.08.27.27,0,0,1,.13,0H12.3a.32.32,0,0,1,.25.1.36.36,0,0,1,.09.24Z"/><path fill="#fff" d="M8.07,14.1a.51.51,0,0,1,0,.13.44.44,0,0,1-.08.11l-.11.08-.13,0h-2l-.13,0-.11-.08a.41.41,0,0,1-.08-.24V12.2a.27.27,0,0,1,0-.13.36.36,0,0,1,.07-.1.39.39,0,0,1,.1-.08l.13,0h2A.31.31,0,0,1,8,12a.39.39,0,0,1,.08.1.51.51,0,0,1,0,.13ZM8,10.17a.17.17,0,0,1,0,.12.41.41,0,0,1-.07.11.4.4,0,0,1-.23.08H5.85a.31.31,0,0,1-.34-.31L5.36,3.4a.36.36,0,0,1,.09-.24.23.23,0,0,1,.11-.08.27.27,0,0,1,.13,0H7.8a.35.35,0,0,1,.25.1.36.36,0,0,1,.09.24Z"/></svg>';
const GREATI='<svg viewBox="0 0 18 19"><path opacity=".3" d="M9,.5a9,9,0,1,0,9,9A9,9,0,0,0,9,.5Z"/><path fill="#749BBF" d="M9,0a9,9,0,1,0,9,9A9,9,0,0,0,9,0Z"/><path opacity=".2" d="M10.32,14.6a.27.27,0,0,1,0,.13.44.44,0,0,1-.08.11l-.11.08-.13,0H8l-.13,0-.11-.08a.41.41,0,0,1-.08-.24V12.7a.27.27,0,0,1,0-.13.36.36,0,0,1,.07-.1.39.39,0,0,1,.1-.08l.13,0h2a.31.31,0,0,1,.24.1.39.39,0,0,1,.08.1.51.51,0,0,1,0,.13Zm-.12-3.93a.17.17,0,0,1,0,.12.41.41,0,0,1-.07.11.4.4,0,0,1-.23.08H8.1a.31.31,0,0,1-.34-.31L7.61,3.9a.36.36,0,0,1,.09-.24.23.23,0,0,1,.11-.08.27.27,0,0,1,.13,0h2.11a.32.32,0,0,1,.25.1.36.36,0,0,1,.09.24Z"/><path fill="#fff" d="M10.32,14.1a.27.27,0,0,1,0,.13.44.44,0,0,1-.08.11l-.11.08-.13,0H8l-.13,0-.11-.08a.41.41,0,0,1-.08-.24V12.2a.27.27,0,0,1,0-.13.36.36,0,0,1,.07-.1.39.39,0,0,1,.1-.08l.13,0h2a.31.31,0,0,1,.24.1.39.39,0,0,1,.08.1.51.51,0,0,1,0,.13Zm-.12-3.93a.17.17,0,0,1,0,.12.41.41,0,0,1-.07.11.4.4,0,0,1-.23.08H8.1a.31.31,0,0,1-.34-.31L7.61,3.4a.36.36,0,0,1,.09-.24.23.23,0,0,1,.11-.08.27.27,0,0,1,.13,0h2.11a.32.32,0,0,1,.25.1.36.36,0,0,1,.09.24Z"/></svg>';
const badge=c=>c==="bril"?`<span class="cb cb-svg">${BRILI}</span>`:c==="great"?`<span class="cb cb-svg">${GREATI}</span>`:c==="best"?`<span class="cb cb-svg">${BESTI}</span>`:c==="good"?`<span class="cb cb-svg">${GOODI}</span>`:c==="book"?`<span class="cb cb-svg">${BOOKI}</span>`:c==="blun"?`<span class="cb cb-svg">${BLUNI}</span>`:c==="inacc"?`<span class="cb cb-svg">${INACI}</span>`:c==="miss"?`<span class="cb cb-svg">${MISSI}</span>`:`<span class="cb cb-${c}">${MARK[c][0]}</span>`;
const MARK={bril:["!!","Brilliant"],great:["!","Great move"],book:["","Book move"],best:["★","Best move"],good:["✓","Good move"],inacc:["?!","Inaccuracy"],miss:["✕","Miss"],blun:["??","Blunder"]};
function renderMoves(){
  const h=hist();let html="";const mx=Math.max(1,...st.times.filter(x=>x!==undefined));
  const GL={w:{n:"♞",b:"♝",r:"♜",q:"♛",k:"♚"},b:{n:"♘",b:"♗",r:"♖",q:"♕",k:"♔"}};
  const cell=j=>{if(j<0||j>=h.length)return `<span class="mvc empty"></span>`;const m=h[j];const c=st.mode==="fen"?st.fcls[j+1]||null:st.anal&&st.anal.cls?st.anal.cls[j+1]:null;const isFig=m.piece!=="p"&&!m.san.startsWith("O-");const san=isFig?m.san.slice(1):m.san;const fig=isFig?`<span class="fig">${GL[m.color][m.piece]}</span>`:"";const show=c==="best"||c==="blun"||c==="bril"||c==="great";
    return `<span class="mvc${c?" t-"+c:""}" data-ply="${j+1}"><span class="cbs">${show?badge(c):""}</span><span class="mvx${st.cv<0&&st.view===j+1?" cur":""}">${fig}${san}</span></span>`};
  const tb=j=>{const t=st.times[j];if(j<0||j>=h.length||t===undefined)return `<span class="tb"></span>`;return `<span class="tb"><span class="tbw"><span class="tbar ${h[j].color}" style="width:${Math.max(4,Math.round(t/mx*100))}%"></span></span><span class="tv">${fmtT(t)}</span></span>`};
  const o=sOff(),n0=sNum();
  const vline=(v,vi)=>{const t0=fenAt(v.base).turn();return `<div class="mvvar">${v.moves.map((m,t)=>{const w=(t0==="w")===(t%2===0),num=plyNo(v.base+1+t);return `${w?`<span class="vn">${num}.</span>`:t===0?`<span class="vn">${num}...</span>`:""}<span class="vm${st.cv===vi&&st.ck===t+1?" cur":""}" data-v="${vi}" data-k="${t+1}">${m}</span>`}).join("")}</div>`};
  for(let k=0;k<h.length+o;k+=2){html+=`<div class="mvr${(k/2)%2?" alt":""}"><span class="mvn">${n0+k/2}.</span>${cell(k-o)}${cell(k+1-o)}<span class="mvts">${tb(k-o)}${tb(k+1-o)}</span></div>`;
    st.vars.forEach((v,vi)=>{if(v.base===k-o||v.base===k-o+1)html+=vline(v,vi)})}
  st.vars.forEach((v,vi)=>{if(v.base>=h.length)html+=vline(v,vi)});
  const mv=$("plMoves");const st0=mv.scrollTop;mv.innerHTML=html;mv.scrollTop=st0;
  {const BK=window.OPENINGS||{};const g=newG();let nm="";for(const m of h){g.move(m.san);const k=g.fen().split(" ").slice(0,4).join(" ");if(BK[k])nm=BK[k]}const el=$("plOpen");el.textContent=nm;el.style.display=nm?"":"none"}
  const cur=mv.querySelector(".cur");if(cur){const r=(cur.closest(".mvr,.mvvar")||cur).getBoundingClientRect(),b=mv.getBoundingClientRect();if(r.top<b.top)mv.scrollTop-=b.top-r.top;else if(r.bottom>b.bottom)mv.scrollTop+=r.bottom-b.bottom}else if(st.cv<0&&st.view===h.length)mv.scrollTop=mv.scrollHeight;
}
const fmtT=ms=>{const s=ms/1000;if(s<60)return s.toFixed(1)+"s";const m=Math.floor(s/60);return m+":"+String(Math.floor(s%60)).padStart(2,"0")};
const fmt=ms=>{ms=Math.max(0,ms);const s=ms/1000;if(s<10)return s.toFixed(1);const m=Math.floor(s/60),ss=Math.floor(s%60);return `${m}:${String(ss).padStart(2,"0")}`};
function renderClocks(){
  const top=st.flip?"w":"b",bot=st.flip?"b":"w";
  [["plClkTop",top],["plClkBot",bot]].forEach(([id,c])=>{const el=$(id);el.querySelector(".n").textContent=c===st.user?"You":"Engine";el.querySelector(".t").textContent=fmt(st.clock[c]);el.classList.toggle("on",st.running&&!st.over&&st.game.turn()===c);el.classList.toggle("low",st.clock[c]<20000)});
}
function renderStatus(){
  let t;
  if(st.over)t=st.over;
  else if(st.analyzing)t="";
  else if(st.thinking)t="Engine is thinking…";
  else t=st.game.turn()===st.user?"Your move":"Engine to move";
  if(st.analyzing)t=st.analyzing;
  const pm=st.mode==="play";if(!pm)t=st.analyzing||(st.mode==="fen"?"Analysis board":st.title);
  {const P=$("plStatus");if(!pm&&st.mode==="pgn"&&st.vs&&!st.analyzing)P.innerHTML=vsH(st.vs.w,st.vs.b);else P.textContent=t}$("plLvl").innerHTML=pm?`Engine <b>${st.gameElo||st.elo}</b>`:"";
  $("plResign").parentElement.style.display=st.over||!pm?"none":"";const dn=!!(st.anal&&st.anal.done);$("plAgain").style.display=pm&&st.over&&!dn?"":"none";$("plAgainTop").style.display=!pm||st.over&&dn?"":"none";
  $("plAnalyze").style.display=pm&&st.over&&!(st.anal&&st.anal.done)?"":"none";$("plCopy").style.display=hist().length&&(st.over||!pm)?"":"none";$("plAnalyze").disabled=!!st.analyzing||hist().length===0;
}
function renderAnal(){
  const box=$("plAnal");const a=st.anal;
  box.style.display=a&&a.done?"":"none";
  if(!a||!a.done)return;
  const h=hist(),n=h.length,W=300,H=70;
  const pts=[];for(let i=0;i<=n;i++){const e=a[i];const p=e?(e.mate!==undefined?(e.cp>0?100:0):winp(e.cp)):50;pts.push([n?i/n*W:0,H-p/100*H])}
  const line=pts.map(p=>p.map(v=>v.toFixed(1)).join(",")).join(" ");
  const x=n?st.view/n*W:0;
  $("plGraph").innerHTML=`<polygon class="g-area" points="0,${H} ${line} ${W},${H}"/><line class="g-mid" x1="0" y1="${H/2}" x2="${W}" y2="${H/2}"/><polyline class="g-line" points="${line}"/><line class="g-cur" x1="${x}" y1="0" x2="${x}" y2="${H}"/><line class="g-hov" id="gHov" x1="0" y1="0" x2="0" y2="${H}" style="display:none"/>`;
  const c=a.cls[st.view],bm=a.bestSan[st.view];
  $("plBest").innerHTML=st.view>0&&(c==="book"||c==="best"||c==="bril"||c==="great")?`<span class="${c}">${MARK[c][1]}</span>`:st.view>0&&c?`<span class="${c}">${MARK[c][1]}</span>${bm?`, best was <b>${bm}</b>`:""}`:"";
  renderExplain();
  const sum=(col,lab)=>{const cnt={bril:0,great:0,best:0,good:0,inacc:0,miss:0,blun:0,book:0};for(let i=1;i<=n;i++){if(h[i-1].color===col&&a.cls[i])cnt[a.cls[i]]++}
    return `<div>${lab}: ${cnt.bril?`<span class="bril">${cnt.bril} brilliant</span> · `:""}${cnt.great?`<span class="great">${cnt.great} great</span> · `:""}<span class="best">${cnt.best} best</span> · <span class="good">${cnt.good} good</span> · <span class="inacc">${cnt.inacc} inaccuracies</span> · <span class="miss">${cnt.miss} misses</span> · <span class="blun">${cnt.blun} blunders</span></div>`};
  $("plSum").innerHTML=st.mode==="pgn"?sum("w","White")+sum("b","Black"):sum(st.user,"You");
}

const RES=()=>/you win/i.test(st.over)?(st.user==="w"?"1-0":"0-1"):/engine wins|you resigned/i.test(st.over)?(st.user==="w"?"0-1":"1-0"):/draw/i.test(st.over)?"1/2-1/2":"*";
function buildPgn(){
  const h=hist(),d=new Date(),dt=`${d.getFullYear()}.${String(d.getMonth()+1).padStart(2,"0")}.${String(d.getDate()).padStart(2,"0")}`;
  const hd=st.mode==="play"?{Event:"Chess Crack vs Stockfish",Date:dt,White:st.user==="w"?"You":`Stockfish (${st.gameElo})`,Black:st.user==="b"?"You":`Stockfish (${st.gameElo})`,Result:RES(),TimeControl:st.tc==="none"?"-":TCS[st.tc].join("+")}:Object.assign({Event:st.title||"Analysis",Date:dt},st.hd||{});
  hd.Result=hd.Result||"*";if(st.startFen!==STD){hd.SetUp="1";hd.FEN=st.startFen}
  let s=Object.entries(hd).map(([k,v])=>`[${k} "${v}"]`).join("\n")+"\n\n";
  h.forEach((m,i)=>{const n=plyNo(i+1);if(m.color==="w")s+=`${n}. `;else if(i===0)s+=`${n}... `;s+=m.san+" "});
  return s+hd.Result;
}
const loadGames=()=>{try{return JSON.parse(localStorage.getItem("cc_games")||"[]")}catch(e){return []}};
const saveGames=a=>{a=a.slice();while(a.length){try{localStorage.setItem("cc_games",JSON.stringify(a));return}catch(e){a.pop()}}try{localStorage.setItem("cc_games","[]")}catch(e){}};let delAsk=null;
function packAnal(a){const ev=[];for(let i=0;i<=hist().length;i++)ev.push(a[i]);return {cls:a.cls,bestSan:a.bestSan,bestUci:a.bestUci||[],ev}}
function storeGame(){const a=loadGames(),id=Date.now();a.unshift({id,date:id,user:st.user,elo:st.gameElo,tc:st.tc,res:RES(),msg:st.over,moves:hist().map(m=>m.san),times:st.times.slice()});saveGames(a);st.gid=id;renderSaved()}
function storeImport(){
  const a=loadGames(),mv=hist().map(m=>m.san),W=st.hd.White||"White",Bk=st.hd.Black||"Black",res=st.hd.Result||"*";
  const ex=a.find(g=>g.kind==="pgn"&&g.white===W&&g.black===Bk&&g.startFen===st.startFen&&g.moves.join(" ")===mv.join(" "));
  if(ex){st.gid=ex.id;if(!(ex.times&&ex.times.length)&&st.times.length){ex.times=st.times.slice();saveGames(a)}return ex}
  const m=/^(\d{4})\.(\d{2})\.(\d{2})$/.exec(st.hd.Date||""),id=Date.now();
  a.unshift({id,kind:"pgn",date:m?new Date(+m[1],+m[2]-1,+m[3]).getTime():id,white:W,black:Bk,res,startFen:st.startFen,moves:mv,times:st.times.slice()});saveGames(a);st.gid=id;return null;
}
function setLastWin(){const g=loadGames().filter(g=>!g.kind&&g.elo&&g.res===(g.user==="w"?"1-0":"0-1")).sort((a,b)=>b.elo-a.elo)[0],e=g?Math.min(3100,g.elo+100):1000;st.elo=e;$("plStr").value=e;$("plElo").textContent=e;$("plLw").textContent=g?`Best win: ${g.elo}`:""}
const esc=s=>String(s).replace(/[&<>"]/g,c=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;"})[c]);
const vsH=(w,b)=>`<span class="pw">${esc(w)}</span> vs <span class="pb">${esc(b)}</span>`;
function renderSaved(){
  const a=loadGames(),box=$("plSaved");box.style.display=a.length?"":"none";if(!a.length)return;
  $("plGList").innerHTML=a.map(g=>{if(g.kind==="pgn"){const d=new Date(g.date),rs=g.res==="1/2-1/2"?"½":g.res==="*"?"–":g.res;
      return `<div class="pl-gi" data-gid="${g.id}"><span class="gr gr-p">${rs}</span><span class="gm">${vsH(g.white,g.black)} · ${Math.ceil(g.moves.length/2)} move${Math.ceil(g.moves.length/2)===1?"":"s"}</span><span class="gd">${d.getDate()} ${d.toLocaleString("en",{month:"short"})}</span>${delAsk===g.id?`<span class="gq">Delete?</span><button class="pl-btn gy" data-yes="${g.id}">Delete</button><button class="pl-btn gn" data-no="1">Cancel</button>`:`<button class="gx" data-del="${g.id}" title="Delete">×</button>`}</div>`}
    const win=g.res===(g.user==="w"?"1-0":"0-1"),r=g.res==="1/2-1/2"?"d":g.res==="*"?"u":win?"w":"l";const d=new Date(g.date);
    return `<div class="pl-gi" data-gid="${g.id}"><span class="gr r-${r}">${{w:"W",l:"L",d:"D",u:"–"}[r]}</span><span class="gm">${g.user==="w"?vsH("You",`Stockfish (${g.elo})`):vsH(`Stockfish (${g.elo})`,"You")} · ${Math.ceil(g.moves.length/2)} move${Math.ceil(g.moves.length/2)===1?"":"s"}</span><span class="gd">${d.getDate()} ${d.toLocaleString("en",{month:"short"})}</span>${delAsk===g.id?`<span class="gq">Delete?</span><button class="pl-btn gy" data-yes="${g.id}">Delete</button><button class="pl-btn gn" data-no="1">Cancel</button>`:`<button class="gx" data-del="${g.id}" title="Delete">×</button>`}</div>`}).join("");
}
function openSaved(id){
  const r=loadGames().find(g=>g.id===id);if(!r)return;const g=new Chess(r.startFen||STD);try{r.moves.forEach(m=>g.move(m))}catch(e){return}
  closePromo();resetVars();st.gen++;st.analGen=(st.analGen||0)+1;
  st.mode="pgn";st.startFen=STD;st.arrows=[];st.marks={};st.game=g;st.user=r.user;st.flip=r.user==="b";st.gameElo=r.elo;st.running=false;st.thinking=false;st.sel=null;st.analyzing=false;st.times=(r.times||[]).slice();st.phase="game";st.gid=r.id;
  if(r.kind==="pgn"){st.startFen=r.startFen||STD;st.user="w";st.flip=false;st.title=`${r.white} vs ${r.black}`;st.vs={w:r.white,b:r.black};st.hd={White:r.white,Black:r.black,Result:r.res};st.over=r.res!=="*"?r.res:"Imported game"}
  else{st.title=`You vs Stockfish (${r.elo})`;st.vs=r.user==="w"?{w:"You",b:`Stockfish (${r.elo})`}:{w:`Stockfish (${r.elo})`,b:"You"};st.hd={White:r.user==="w"?"You":`Stockfish (${r.elo})`,Black:r.user==="b"?"You":`Stockfish (${r.elo})`,Result:r.res};st.over=r.msg||r.res}st.view=hist().length;
  if(r.anal){const a={cls:r.anal.cls,bestSan:r.anal.bestSan,bestUci:r.anal.bestUci,done:true};r.anal.ev.forEach((e,i)=>a[i]=e);st.anal=a;render()}else{st.anal=null;render();analyze()}
}
function finish(msg){setTimeout(()=>sfx("game-end",()=>chime("end")),250);st.over=msg;st.running=false;st.thinking=false;st.gen++;try{playEng&&playEng.send("stop")}catch(e){}if(st.mode==="play"&&hist().length)storeGame();render()}
function checkEnd(){
  const g=st.game;if(!g.isGameOver())return false;
  const youWin=g.turn()!==st.user;
  if(g.isCheckmate())finish(youWin?"Checkmate, you win":"Checkmate, engine wins");
  else if(g.isStalemate())finish("Draw by stalemate");
  else if(g.isThreefoldRepetition())finish("Draw by repetition");
  else if(g.isInsufficientMaterial())finish("Draw, insufficient material");
  else finish("Draw by 50-move rule");
  return true;
}
let AC=null;
const SFX={};
function sfx(name,fb){if(!SET.sound)return;let a=SFX[name];if(a===false){fb&&fb();return}
if(!a){a=SFX[name]=new Audio("lib/sounds/"+name+".mp3");a.addEventListener("error",()=>{SFX[name]=false},{once:true})}
try{a.currentTime=0}catch(e){}a.play().catch(()=>{if(SFX[name]===false)fb&&fb()})}
function snd(cap){sfx(cap?"capture":"move",()=>cap?sfx("move",()=>synth(cap)):synth(cap))}
function mvSnd(mv){if(/[+#]/.test(mv.san))sfx("check",()=>snd(!!mv.captured));else if(mv.promotion)sfx("promote",()=>snd(!!mv.captured));else if(/[kq]/.test(mv.flags))sfx("castle",()=>snd(false));else snd(!!mv.captured)}
function chime(kind){if(!SET.sound)return;try{AC=AC||new (window.AudioContext||window.webkitAudioContext)();const t=AC.currentTime+.02;
const n=(f,at,d,v)=>{const o=AC.createOscillator(),g=AC.createGain();o.type="sine";o.frequency.value=f;g.gain.setValueAtTime(.0001,t+at);g.gain.exponentialRampToValueAtTime(v,t+at+.02);g.gain.exponentialRampToValueAtTime(.0001,t+at+d);o.connect(g).connect(AC.destination);o.start(t+at);o.stop(t+at+d+.05)};
if(kind==="start"){n(523.25,0,.35,.18);n(783.99,.12,.5,.16)}else{n(783.99,0,.4,.16);n(659.25,.13,.4,.15);n(523.25,.26,.75,.17)}}catch(e){}}
function synth(cap){if(!SET.sound)return;try{AC=AC||new (window.AudioContext||window.webkitAudioContext)();const t=AC.currentTime;const tone=(f,d,at,v)=>{const o=AC.createOscillator(),g=AC.createGain();o.type="triangle";o.frequency.value=f;g.gain.setValueAtTime(v,t+at);g.gain.exponentialRampToValueAtTime(.001,t+at+d);o.connect(g).connect(AC.destination);o.start(t+at);o.stop(t+at+d)};if(cap){tone(220,.09,0,.35);tone(160,.12,.05,.3)}else tone(420,.07,0,.25)}catch(e){}}
function applied(mv){
  st.arrows=[];st.marks={};
  if(st.mode!=="play"){if(!st.noAnim)st.anim=mv;st.noAnim=false;mvSnd(mv);st.view=hist().length;st.sel=null;if(st.mode==="fen"){const ply=hist().length;liveClassify(ply,mv.before||fenAt(ply-1).fen(),mv.after||st.game.fen(),mv)}render();return}
  if(!st.noAnim)st.anim=mv;st.noAnim=false;
  mvSnd(mv);
  const nowT=performance.now();st.times[hist().length-1]=nowT-st.turnStart;st.turnStart=nowT;
  const c=mv.color;st.clock[c]+=TCS[st.tc][1]*1000;
  if(!st.running&&hist().length>=1)st.running=true;
  st.view=hist().length;st.sel=null;
  if(!checkEnd()){render();maybeEngine()}
}
function tryMove(from,to,promotion,quiet){
  const G=act();if(!G)return false;
  const legal=G.moves({square:from,verbose:true}).filter(m=>m.to===to);
  if(!legal.length){if(!quiet)sfx("illegal");return false}
  if(legal.some(m=>m.promotion)&&!promotion){showPromo(from,to);return true}
  if(canEx()&&!(st.mode==="fen"&&st.cv<0&&live())){exMove(G,from,to,promotion);return true}
  const mv=st.game.move({from,to,promotion:promotion||undefined});
  applied(mv);return true;
}
function exMove(G,from,to,promotion){
  const mv=G.move({from,to,promotion:promotion||undefined}),san=mv.san,h=hist();
  if(st.cv<0){const i=st.view;
    if(i<h.length&&h[i].san===san)st.view=i+1;
    else{let x=st.vars.findIndex(v=>v.base===i&&v.moves[0]===san);if(x<0){st.vars.push({base:i,moves:[san]});x=st.vars.length-1}st.cv=x;st.ck=1}}
  else{const v=st.vars[st.cv];
    if(st.ck<v.moves.length&&v.moves[st.ck]===san)st.ck++;
    else if(st.ck===v.moves.length){v.moves.push(san);st.ck++}
    else{const pre=v.moves.slice(0,st.ck).concat(san);let x=st.vars.findIndex(w=>w.base===v.base&&w.moves.length>=pre.length&&pre.every((m,k)=>w.moves[k]===m));if(x<0){st.vars.push({base:v.base,moves:pre});x=st.vars.length-1}st.cv=x;st.ck=pre.length}}
  st.arrows=[];st.marks={};st.sel=null;st.noAnim?st.noAnim=false:st.anim={from:mv.from,to:mv.to};mvSnd(mv);render();
}
function step(d){
  if(st.cv<0){go(st.view+d);return}
  const v=st.vars[st.cv];st.arrows=[];st.marks={};closePromo();st.sel=null;
  if(d>0){if(st.ck<v.moves.length){st.ck++;const l=curState().last;st.anim={from:l.from,to:l.to}}}
  else{const l=curState().last;st.anim={from:l.to,to:l.from};if(st.ck>1)st.ck--;else{st.cv=-1;st.ck=0;st.view=v.base}}
  render();
}
function closePromo(){$("plPromo").classList.remove("on");st.promo=null}
function showPromo(from,to){
  const el=$("plPromo");const c=(act()||st.game).turn();
  el.innerHTML=["q","r","b","n"].map(p=>`<button data-p="${p}"><img src="${PIECES[c==="w"?p.toUpperCase():p]}" alt=""></button>`).join("");
  el.classList.add("on");st.promo={from,to};
  el.onclick=e=>{const b=e.target.closest("button");closePromo();if(!b){st.sel=null;render();return}tryMove(from,to,b.dataset.p)};
}

async function maybeEngine(){
  if(st.mode!=="play"||st.over||st.game.turn()===st.user)return;
  st.thinking=true;renderStatus();
  const gen=++st.gen;const eng=getPlay();await eng.ready;
  if(gen!==st.gen)return;
  if(st.elo>=1320){eng.send("setoption name Skill Level value 20");eng.send("setoption name UCI_LimitStrength value true");eng.send("setoption name UCI_Elo value "+st.elo)}
  else{eng.send("setoption name UCI_LimitStrength value false");eng.send("setoption name Skill Level value "+Math.round((st.elo-1000)/320*4))}
  eng.send("position fen "+st.game.fen());
  const [,inc]=TCS[st.tc];
  if(st.elo<1320){eng.send("go depth "+(1+Math.round((st.elo-1000)/320*4)))}else{const mt=st.tc==="none"?1000:Math.round(Math.min(2500,Math.max(300,st.clock[st.game.turn()]/40+inc*400)));eng.send("go movetime "+mt)}
  const l=await eng.wait(x=>x.startsWith("bestmove"));
  if(gen!==st.gen)return;
  st.thinking=false;
  const u=l.split(" ")[1];if(!u||u==="(none)"){render();return}
  const mv=st.game.move({from:u.slice(0,2),to:u.slice(2,4),promotion:u[4]});
  applied(mv);
}

let tickT=performance.now();
setInterval(()=>{const now=performance.now(),dt=now-tickT;tickT=now;if(!st.running||st.over||st.tc==="none")return;const c=st.game.turn(),pc=st.clock[c];st.clock[c]-=dt;if(c===st.user&&pc>=10000&&st.clock[c]<10000)sfx("time");if(st.clock[c]<=0){st.clock[c]=0;finish(c===st.user?"Time out, engine wins":"Time out, you win");return}renderClocks()},100);

function newGame(){
  const g=new Chess();st.startFen=STD;st.mode="play";closePromo();
  st.gen++;try{playEng&&playEng.send("stop");playEng&&playEng.send("ucinewgame")}catch(e){}
  resetVars();st.gid=null;st.hd=null;st.arrows=[];st.marks={};st.gameElo=st.elo;st.game=g;st.user=st.colorPick==="r"?(Math.random()<.5?"w":"b"):st.colorPick;st.flip=st.user==="b";
  const [base]=TCS[st.tc];st.clock={w:base*1000,b:base*1000};st.running=false;st.over=null;st.thinking=false;st.sel=null;st.view=g.history().length;st.anal=null;st.analyzing=false;st.analGen=(st.analGen||0)+1;
  st.times=[];st.turnStart=performance.now();st.phase="game";sfx("game-start",()=>chime("start"));render();maybeEngine();
}
function importGame(){
  const raw=$("plImp").value,err=$("plImpErr");const hd={};raw.replace(/\[(\w+)\s+"([^"]*)"\]/g,(_,k,v)=>{hd[k]=v});
  const sv=raw.replace(/^\s*\[\w+\s+"[^"]*"\]\s*$/gm," ").trim();let g,mode;
  try{
    if(!sv&&!hd.FEN)throw new Error("Paste a PGN or FEN");
    if(/^[1-8pnbrqkPNBRQK]+(\/[1-8pnbrqkPNBRQK]+){7}\s/.test(sv+" ")){try{g=new Chess(sv)}catch(e){throw new Error("Invalid FEN")}st.startFen=g.fen();mode="fen"}
    else{try{g=new Chess(hd.FEN||STD)}catch(e){throw new Error("Invalid FEN header")}st.startFen=g.fen();mode="pgn";
      let s2=sv;while(/\([^()]*\)/.test(s2))s2=s2.replace(/\([^()]*\)/g," ");
      const clk=[];for(const t of (s2.replace(/\$\d+/g," ").match(/\{[^}]*\}|[^\s{}]+/g)||[])){
        if(t[0]==="{"){const c=/%clk\s+(\d+):(\d+):(\d+(?:\.\d+)?)/.exec(t);if(c&&g.history().length)clk[g.history().length-1]=(+c[1]*3600+ +c[2]*60+ +c[3])*1000;continue}
        const mt=t.replace(/^\d+\.(\.\.)?/,"");if(!mt||/^(1-0|0-1|1\/2-1\/2|\*)$/.test(mt))continue;try{g.move(mt)}catch(e){throw new Error("Illegal move: "+mt)}}
      var pgnTimes=[];const tcm=/^(\d+)(?:\+(\d+))?/.exec(hd.TimeControl||""),base=tcm?+tcm[1]*1000:null,inc=tcm&&tcm[2]?+tcm[2]*1000:0,prev={};
      g.history({verbose:true}).forEach((m,i)=>{const c=clk[i];if(c===undefined)return;const pv=prev[m.color]!==undefined?prev[m.color]:base;if(pv!==null&&pv!==undefined)pgnTimes[i]=Math.max(0,pv-c+inc);prev[m.color]=c});
      if(!g.history().length)throw new Error("No moves found")}
  }catch(e){err.textContent=e.message;return}
  err.textContent="";$("plImp").value="";closePromo();
  st.gen++;st.analGen=(st.analGen||0)+1;try{playEng&&playEng.send("stop")}catch(e){}
  resetVars();st.mode=mode;st.arrows=[];st.marks={};st.game=g;st.user="w";st.flip=false;st.running=false;st.thinking=false;st.sel=null;st.anal=null;st.analyzing=false;st.times=mode==="pgn"&&typeof pgnTimes!=="undefined"?pgnTimes:[];st.phase="game";
  st.title=hd.White&&hd.Black?`${hd.White} vs ${hd.Black}`:"Imported game";st.vs=hd.White&&hd.Black?{w:hd.White,b:hd.Black}:null;st.gid=null;st.hd={};["White","Black","Result","Date","Event"].forEach(k=>{if(hd[k])st.hd[k]=hd[k]});
  if(mode==="pgn"){st.over=hd.Result&&hd.Result!=="*"?hd.Result:"Imported game";st.view=0;const ex=storeImport();
    if(ex&&ex.anal){const a={cls:ex.anal.cls,bestSan:ex.anal.bestSan,bestUci:ex.anal.bestUci,done:true};ex.anal.ev.forEach((e,i)=>a[i]=e);st.anal=a;render()}else{render();analyze()}}else{st.over=null;st.view=hist().length;render()}
}

async function evalPos(eng,fen,depth,mpv,tm){
  const pos=new Chess(fen);const stm=pos.turn(),sgn=stm==="w"?1:-1;
  if(pos.isCheckmate())return {e:{mate:0,cp:stm==="w"?-100000:100000},lines:[]};
  if(pos.isDraw())return {e:{cp:0},lines:[]};
  mpv=Math.min(mpv||1,pos.moves().length);
  eng.send("setoption name MultiPV value "+mpv);eng.send("position fen "+fen);eng.send("go depth "+depth+" movetime "+(tm||700));
  const L=[];const off=eng.on(l=>{if(l.startsWith("info")&&l.includes(" score ")&&!l.includes("bound")){const k=+((l.match(/ multipv (\d+)/)||[0,1])[1]);L[k-1]=l}});
  const bl=await eng.wait(l=>l.startsWith("bestmove"));off();
  const pe=l=>{const m=l&&l.match(/score (cp|mate) (-?\d+)/);if(!m)return {cp:0};const v=+m[2];return m[1]==="mate"?{mate:v*sgn,cp:(v>0?1:-1)*sgn*100000}:{cp:v*sgn}};
  const lines=L.filter(Boolean).map(l=>({e:pe(l),pv:((l.match(/ pv (.*)$/)||[0,""])[1]).split(" ").filter(Boolean)}));
  const u=bl.split(" ")[1];return {e:lines[0]?lines[0].e:{cp:0},u:u&&u!=="(none)"?u:null,lines};
}
function classifyMove(o){
  const cl=v=>Math.max(-1e5,Math.min(1e5,v)),wpm=(ev,c)=>{const w=winp(cl(ev.cp));return c==="w"?w:100-w};
  const before=wpm(o.eB,o.mover),after=wpm(o.eA,o.mover),drop=before-after,isBest=o.best===o.played;
  if(isBest||drop<2){
    const VAL={p:1,n:3,b:3,r:5,q:9,k:0},mat=(fen,c)=>{let s=0;new Chess(fen).board().forEach(r=>r.forEach(q=>{if(q)s+=(q.color===c?1:-1)*VAL[q.type]}));return s};
    let sac=false;try{const t=new Chess(o.fenA);(o.pvA||[]).slice(0,2).forEach(u=>t.move({from:u.slice(0,2),to:u.slice(2,4),promotion:u[4]}));sac=mat(o.fenB,o.mover)-mat(t.fen(),o.mover)>=2}catch(e){}
    let forcing=false;try{const m=new Chess(o.fenB).move({from:o.played.slice(0,2),to:o.played.slice(2,4),promotion:o.played[4]});forcing=!!m.captured||/[+#]/.test(m.san)}catch(e){}
    const hidden=!forcing&&!!(o.sh&&o.sh.length&&!o.sh.includes(o.played));
    const easy=!!(o.l2&&o.l2.mate===undefined&&wpm(o.l2,o.mover)>=winp(500));
    if(!easy&&after>=winp(-100)&&(sac||hidden))return "bril";
    if(isBest&&o.l2&&before-wpm(o.l2,o.mover)>=20)return "great";
  }
  if(isBest)return "best";
  const bad=drop>=25?"blun":drop>=10?"inacc":null;
  return bad&&(o.prev==="blun"||o.prev==="miss")?"miss":bad||"good";
}
let fq=Promise.resolve();
function liveClassify(ply,fenB,fenA,mv){
  const BK=window.OPENINGS||{};if(fenA.split(" ").slice(0,4).join(" ") in BK){st.fcls[ply]="book";return}
  const tok=st.fTok;
  fq=fq.then(async()=>{if(tok!==st.fTok)return;const eng=getAnal();await eng.ready;eng.send("setoption name UCI_LimitStrength value false");
    const b=await evalPos(eng,fenB,20,2),sh=await evalPos(eng,fenB,8,2,300),a=await evalPos(eng,fenA,20,1);if(tok!==st.fTok)return;
    st.fcls[ply]=classifyMove({eB:b.e,eA:a.e,best:b.u,l2:b.lines[1]?b.lines[1].e:null,pvA:a.lines[0]?a.lines[0].pv:[],sh:sh.lines.map(x=>x.pv[0]),played:mv.from+mv.to+(mv.promotion||""),mover:mv.color,fenB,fenA,prev:st.fcls[ply-1]});
    render()}).catch(()=>{});
}
const EXC={};let exForm=false;
const gKey=()=>{try{return localStorage.getItem("cc_gemini_key")||""}catch(e){return ""}};
function exCtx(){
  const a=st.anal,i=st.view;if(!a||!a.done||st.cv>=0||i<1)return null;
  const h=hist(),m=h[i-1],fb=fenAt(i-1).fen();return {a,i,m,fb,key:fb+"|"+m.san};
}
const sanLine=(fen,pv,n)=>{const t=new Chess(fen),o=[];for(const u of (pv||[]).slice(0,n)){try{const m=t.move({from:u.slice(0,2),to:u.slice(2,4),promotion:u[4]});o.push(m.san)}catch(e){break}}return o.join(" ")};
const evTxt=e=>!e?"unknown":e.mate!==undefined?(e.mate===0?"checkmate":(e.mate>0?"White":"Black")+" mates in "+Math.abs(e.mate)):(e.cp>=0?"+":"")+(e.cp/100).toFixed(2)+" (from White's view)";
function renderExplain(){
  const w=$("plExW"),c=exCtx();if(!w)return;if(!c){w.innerHTML="";return}
  const r=EXC[c.key];
  if(exForm&&!gKey()){w.innerHTML=`<div class="ex-k"><input class="pl-in" id="exKey" type="password" placeholder="Gemini API key" spellcheck="false"><button class="pl-btn" id="exSave">Save</button></div><a class="ex-l" href="https://aistudio.google.com/apikey" target="_blank" rel="noopener">Get a free key from Google AI Studio</a>`;return}
  if(!r){w.innerHTML=`<button class="pl-btn ex-b" id="exGo">Explain</button>`;return}
  if(r.loading){w.innerHTML=`<div class="ex-t ex-m">Thinking…</div>`;return}
  if(r.err){w.innerHTML=`<div class="ex-t ex-e">${esc(r.err)}</div><button class="pl-btn ex-b" id="exGo">Try again</button>${/key/i.test(r.err)?`<button class="pl-btn ex-b" id="exForget">Change key</button>`:""}`;return}
  w.innerHTML=`<div class="ex-t">${esc(r.text).replace(/\*\*(.+?)\*\*/g,"<b>$1</b>").replace(/\n+/g,"<br>")}</div>`;
}
async function gemini(prompt){
  const call=async model=>{const r=await fetch(`https://generativelanguage.googleapis.com/v1beta/models/${model}:generateContent`,{method:"POST",headers:{"Content-Type":"application/json","x-goog-api-key":gKey()},body:JSON.stringify({contents:[{parts:[{text:prompt}]}],generationConfig:{temperature:.2,maxOutputTokens:200}})});return r};
  let r=await call("gemini-3.5-flash-lite");
  if(r.status===400||r.status===401||r.status===403)throw new Error("Invalid API key");
  if(r.status===429)throw new Error("Free limit reached, try again in a minute");
  if(!r.ok)throw new Error("Gemini error "+r.status);
  const j=await r.json(),t=(((j.candidates||[])[0]||{}).content||{}).parts;const txt=(t||[]).map(x=>x.text||"").join("").trim();if(!txt)throw new Error("Empty answer, try again");return txt;
}
async function explain(){
  const c=exCtx();if(!c)return;if(!gKey()){exForm=true;renderExplain();return}
  const {a,i,m,fb,key}=c,mover=m.color==="w"?"White":"Black",opp=m.color==="w"?"Black":"White",cls=a.cls[i],best=a.bestUci&&a.bestUci[i];
  const played=m.from+m.to+(m.promotion||""),playedIsBest=best===played,fa=fenAt(i).fen();
  EXC[key]={loading:true};renderExplain();
  try{
    const eng=getAnal();await eng.ready;eng.send("setoption name UCI_LimitStrength value false");
    const rb=await evalPos(eng,fb,20,1,1500),ra=await evalPos(eng,fa,20,1,1500);
    const pvB=(rb.lines[0]&&rb.lines[0].pv)||[],pvA=(ra.lines[0]&&ra.lines[0].pv)||[];
    const VAL={p:1,n:3,b:3,r:5,q:9,k:0},NM={p:"pawn",n:"knight",b:"bishop",r:"rook",q:"queen"};
    const run=(fen,pv,n)=>{const t=new Chess(fen),san=[],caps=[];for(const u of pv.slice(0,n)){let mv;try{mv=t.move({from:u.slice(0,2),to:u.slice(2,4),promotion:u[4]})}catch(e){break}san.push(mv.san);if(mv.captured)caps.push(`${mv.color==="w"?"White":"Black"} takes ${NM[mv.captured]} (${mv.san})`)}
      let s=0;t.board().forEach(r=>r.forEach(q=>{if(q)s+=(q.color===m.color?1:-1)*VAL[q.type]}));return {san:san.join(" "),caps,mat:s,mate:t.isCheckmate()}};
    const mat0=(()=>{let s=0;new Chess(fb).board().forEach(r=>r.forEach(q=>{if(q)s+=(q.color===m.color?1:-1)*VAL[q.type]}));return s})();
    const LA=run(fa,pvA,6),LB=run(fb,pvB,6);
    const sw=d=>d===0?"material stays even":d>0?`${mover} ends up ${d} pawn-units ahead of the start`:`${mover} ends up ${-d} pawn-units behind the start`;
    const facts=[`${mover} played ${m.san} (${MARK[cls]?MARK[cls][1]:cls}).`,
      `Line after ${m.san}: ${m.san} ${LA.san}${LA.mate?" (checkmate)":""}. Captures: ${LA.caps.join("; ")||"none"}. Result: ${sw(LA.mat-mat0)}.`,
      playedIsBest?"":`Best was ${(new Chess(fb)).move({from:best.slice(0,2),to:best.slice(2,4),promotion:best[4]}).san}. Line: ${LB.san}${LB.mate?" (checkmate)":""}. Captures: ${LB.caps.join("; ")||"none"}. Result: ${sw(LB.mat-mat0)}.`,
      `Eval before: ${evTxt(rb.e)}. Eval after: ${evTxt(ra.e)}.`].filter(Boolean).join("\n");
    const prompt=`Chess coach. Explain the move in at most 2 short sentences, under 35 words total.
Rules:
- Be concrete: name the key reply and what it wins or threatens, using the given lines and captures (e.g. "after hxg5, Qxg5 attacks the knight on g4 and wins it").
- ${playedIsBest?"Say what the move achieves.":`Say what ${m.san} allows, then what the best move does instead.`}
- No evaluation numbers, no adjectives like "massive" or "terrible", no filler, never comment on missing information.
- Only use facts from the lines below. Standard algebraic notation. Plain text.

${facts}`;
    EXC[key]={text:await gemini(prompt)}
  }catch(e){EXC[key]={err:e.message||"Failed"}}
  renderExplain();
}
$("plExW").addEventListener("click",e=>{const id=e.target.id;
  if(id==="exGo"){const c=exCtx();if(c)delete EXC[c.key];explain()}
  else if(id==="exSave"){const v=$("exKey").value.trim();if(!v)return;try{localStorage.setItem("cc_gemini_key",v)}catch(er){}exForm=false;explain()}
  else if(id==="exForget"){try{localStorage.removeItem("cc_gemini_key")}catch(er){}const c=exCtx();if(c)delete EXC[c.key];exForm=true;renderExplain()}});
async function analyze(){
  const h=hist();if(!h.length)return;resetVars();
  const ag=st.analGen=(st.analGen||0)+1;
  const eng=getAnal();await eng.ready;
  eng.send("setoption name UCI_LimitStrength value false");
  const a={cls:[],bestSan:[]};const n=h.length;const g=newG();const fens=[g.fen()];
  h.forEach(m=>{g.move(m.san);fens.push(g.fen())});
  for(let i=0;i<=n;i++){
    if(ag!==st.analGen)return;
    st.analyzing=`Analyzing ${i}/${n}`;renderStatus();
    const r=await evalPos(eng,fens[i],20,2);a[i]=r.e;const u=r.u;a.l2=a.l2||[];a.pv=a.pv||[];a.sh=a.sh||[];a.l2[i]=r.lines[1]?r.lines[1].e:null;a.pv[i]=r.lines[0]?r.lines[0].pv:[];
    if(i<n){const s=await evalPos(eng,fens[i],8,2,300);a.sh[i+1]=s.lines.map(x=>x.pv[0])}
    if(u&&i<n){try{const t=new Chess(fens[i]);a.bestSan[i+1]=t.move({from:u.slice(0,2),to:u.slice(2,4),promotion:u[4]}).san;a.bestUci=a.bestUci||[];a.bestUci[i+1]=u}catch(e){}}
  }
  const BK=window.OPENINGS||{};let inBook=true;
  for(let i=1;i<=n;i++){
    if(inBook&&(fens[i].split(" ").slice(0,4).join(" ") in BK)){a.cls[i]="book";a.bestSan[i]=null;continue}inBook=false;
    const played=h[i-1].from+h[i-1].to+(h[i-1].promotion||"");
    a.cls[i]=classifyMove({eB:a[i-1],eA:a[i],best:a.bestUci&&a.bestUci[i],l2:a.l2[i-1],pvA:a.pv[i],sh:a.sh[i],played,mover:h[i-1].color,fenB:fens[i-1],fenA:fens[i],prev:a.cls[i-1]});
    if(/^(bril|great|best)$/.test(a.cls[i]))a.bestSan[i]=null;
  }
  a.done=true;st.anal=a;st.analyzing=false;if(st.gid){const L=loadGames(),r=L.find(g=>g.id===st.gid);if(r){r.anal=packAnal(a);saveGames(L)}}render();
}

function go(n){st.arrows=[];st.marks={};closePromo();if(st.cv>=0){const b=st.vars[st.cv].base;st.cv=-1;st.ck=0;st.view=b}const h=hist(),L=h.length,v=Math.max(0,Math.min(L,n));if(v===st.view+1)st.anim={from:h[v-1].from,to:h[v-1].to};else if(v===st.view-1)st.anim={from:h[v].to,to:h[v].from};st.view=v;st.sel=null;render()}

B.addEventListener("contextmenu",e=>e.preventDefault());
const rcCancel=e=>{if(st.promo){st.rcAt=performance.now();e.preventDefault();closePromo();st.sel=null;render();return}if(!(st.drag||st.sel))return;st.rcAt=performance.now();e.preventDefault();if(st.drag){st.drag.cancel=true;st.drag.ghost.remove();const d=st.drag;st.drag=null;st.sel=null;d.cleanup&&d.cleanup();render()}else{st.sel=null;render()}};
addEventListener("mousedown",e=>{if(e.button===2)rcCancel(e)},true);addEventListener("contextmenu",e=>{if(st.drag||st.sel||st.promo)rcCancel(e)},true);
let spDown=false;
const spOk=()=>root.closest(".tabpane").classList.contains("on")&&!/INPUT|TEXTAREA|BUTTON/.test(document.activeElement.tagName);
addEventListener("keydown",e=>{if(e.code==="Space"&&spOk()){e.preventDefault();spDown=true}});
addEventListener("keyup",e=>{if(e.code==="Space")spDown=false});addEventListener("blur",()=>{spDown=false});
B.addEventListener("mousedown",e=>{if(e.button!==2||performance.now()-st.rcAt<60||st.phase!=="game"||(st.over&&!(st.anal&&st.anal.done)))return;const q=e.target.closest(".sq");st.arDown=q?q.dataset.sq:null});
addEventListener("mouseup",e=>{if(e.button!==2||!st.arDown)return;const t=document.elementFromPoint(e.clientX,e.clientY),q=t&&t.closest&&t.closest("#plB .sq"),a=st.arDown;st.arDown=null;
  if(!q)return;if(q.dataset.sq===a){const c=e.shiftKey||spDown?"b":"r";if(st.marks[a]===c)delete st.marks[a];else st.marks[a]=c;render();return}const k=a+q.dataset.sq,i=st.arrows.indexOf(k);if(i<0)st.arrows.push(k);else st.arrows.splice(i,1);render()});
B.addEventListener("pointerdown",e=>{if(e.button!==0)return;const s0=(e.target.closest(".sq")||{}).dataset;if(st.arrows.length||Object.keys(st.marks).length){st.arrows=[];st.marks={};render()}
  const sq=s0&&s0.sq?B.querySelector(`.sq[data-sq="${s0.sq}"]`):null;if(!sq)return;
  if(!canEx()&&!(live()&&userTurn()))return;
  const s=sq.dataset.sq,p=act().get(s);
  if(st.sel&&st.sel!==s&&!mine(p)){if(tryMove(st.sel,s,undefined,true))return}
  if(mine(p)){
    st.sel=s;e.preventDefault();
    const img=sq.querySelector(".pc");const ghost=img.cloneNode();ghost.className="pl-ghost";const w=sq.getBoundingClientRect().width;ghost.style.width=ghost.style.height=w+"px";document.body.appendChild(ghost);
    const mvG=ev=>{ghost.style.left=ev.clientX-w/2+"px";ghost.style.top=ev.clientY-w/2+"px"};mvG(e);
    st.drag={from:s,ghost,moved:false};document.documentElement.classList.add("pl-grabbing");render();
    const hov=q=>{B.querySelectorAll(".sq.hov").forEach(x=>{if(x!==q)x.classList.remove("hov")});if(q)q.classList.add("hov")};
    const move=ev=>{st.drag.moved=true;mvG(ev);const t=document.elementFromPoint(ev.clientX,ev.clientY);hov(t&&t.closest?t.closest("#plB .sq"):null)};
    const up=ev=>{if(!st.drag||ev.button===2)return;hov(null);document.documentElement.classList.remove("pl-grabbing");removeEventListener("pointermove",move);removeEventListener("pointerup",up);ghost.remove();const d=st.drag;st.drag=null;
      const t=document.elementFromPoint(ev.clientX,ev.clientY);const tq=t&&t.closest&&t.closest("#plB .sq");
      if(tq&&tq.dataset.sq!==d.from&&d.moved){st.noAnim=true;if(tryMove(d.from,tq.dataset.sq))return;st.noAnim=false}else if(tq&&tq.dataset.sq!==d.from&&tryMove(d.from,tq.dataset.sq,undefined,true))return;render()};
    st.drag.cleanup=()=>{hov(null);document.documentElement.classList.remove("pl-grabbing");removeEventListener("pointermove",move);removeEventListener("pointerup",up)};addEventListener("pointermove",move);addEventListener("pointerup",up);
  }else{st.sel=null;render()}
});

$("plMoves").addEventListener("click",e=>{const vm=e.target.closest("[data-v]");if(vm){st.cv=+vm.dataset.v;st.ck=+vm.dataset.k;st.arrows=[];st.marks={};st.sel=null;closePromo();render();return}const c=e.target.closest("[data-ply]");if(c)go(+c.dataset.ply)});
root.querySelector(".pl-nav").addEventListener("click",e=>{const b=e.target.closest("button");if(!b)return;const L=hist().length;({first:()=>go(0),prev:()=>step(-1),next:()=>step(1),last:()=>go(L)})[b.dataset.n]()});
{const G=$("plGraph"),T=$("plTip");
const evs=e=>!e?"":e.mate!==undefined?(e.mate===0?"#":(e.cp>0?"+":"-")+"M"+Math.abs(e.mate)):(e.cp>=0?"+":"-")+(Math.abs(e.cp)/100).toFixed(1);
G.addEventListener("mousemove",ev=>{const a=st.anal;if(!a||!a.done)return;const h=hist(),n=h.length;if(!n)return;
const r=G.getBoundingClientRect(),i=Math.max(0,Math.min(n,Math.round((ev.clientX-r.left)/r.width*n))),x=i/n*300;
const L=document.getElementById("gHov");if(L){L.setAttribute("x1",x);L.setAttribute("x2",x);L.style.display=""}
const c=a.cls[i],mv=i?`${plyNo(i)}${h[i-1].color==="w"?".":"..."} ${h[i-1].san}`:"Start";
T.innerHTML=`${i&&c?badge(c):""}<b>${mv}</b><span class="tpe">${evs(a[i])}</span>`;T.classList.add("on");
T.style.left="0px";const pr=T.parentElement.getBoundingClientRect(),px=r.left-pr.left+i/n*r.width,tw=Math.ceil(T.getBoundingClientRect().width);
T.style.left=Math.floor(Math.max(0,Math.min(pr.width-tw-2,px-tw/2)))+"px";T.style.top=(r.top-pr.top-T.offsetHeight-6)+"px"});
G.addEventListener("mouseleave",()=>{T.classList.remove("on");const L=document.getElementById("gHov");if(L)L.style.display="none"})}
$("plGraph").addEventListener("click",e=>{const r=e.currentTarget.getBoundingClientRect();go(Math.round((e.clientX-r.left)/r.width*hist().length))});
addEventListener("keydown",e=>{if(!root.closest(".tabpane").classList.contains("on")||/INPUT|TEXTAREA/.test(document.activeElement.tagName)&&document.activeElement.type!=="range")return;const k={ArrowLeft:()=>step(-1),ArrowRight:()=>step(1),Home:()=>go(0),End:()=>go(hist().length)}[e.key];if(k){e.preventDefault();k()}});
const seg=(id,cb)=>$(id).querySelectorAll("button").forEach(b=>b.addEventListener("click",()=>{$(id).querySelectorAll("button").forEach(x=>x.classList.toggle("on",x===b));cb(b.dataset.v)}));
seg("plColor",v=>{st.colorPick=v});
seg("plTc",v=>{st.tc=v});
$("plStr").addEventListener("input",e=>{st.elo=+e.target.value;$("plElo").textContent=st.elo});
$("plNew").addEventListener("click",newGame);
$("plImp").addEventListener("input",()=>{$("plImpErr").textContent=""});$("plImpGo").addEventListener("click",importGame);
$("plGList").addEventListener("click",e=>{const d=e.target.closest("[data-del]");if(d){delAsk=+d.dataset.del;renderSaved();return}const y=e.target.closest("[data-yes]");if(y){const gid=+y.dataset.yes;saveGames(loadGames().filter(g=>g.id!==gid));try{const dl=JSON.parse(localStorage.getItem("cc_games_del")||"[]");dl.push(gid);localStorage.setItem("cc_games_del",JSON.stringify(dl))}catch(er){}delAsk=null;renderSaved();return}if(e.target.closest("[data-no]")){delAsk=null;renderSaved();return}if(delAsk!==null){delAsk=null;renderSaved();return}const r=e.target.closest("[data-gid]");if(r)openSaved(+r.dataset.gid)});
$("plCopy").addEventListener("click",async()=>{const t=buildPgn(),b=$("plCopy");let ok=false;try{await navigator.clipboard.writeText(t);ok=true}catch(e){const ta=document.createElement("textarea");ta.value=t;document.body.appendChild(ta);ta.select();try{ok=document.execCommand("copy")}catch(e2){}ta.remove()}
  b.textContent=ok?"Copied ✓":"Copy failed";setTimeout(()=>{b.textContent="Copy PGN"},1500)});
renderSaved();setLastWin();
const again=()=>{closePromo();resetVars();renderSaved();setLastWin();st.mode="play";st.startFen=STD;st.gen++;st.analGen=(st.analGen||0)+1;st.phase="setup";st.game=new Chess();st.view=0;st.over=null;st.anal=null;st.analyzing=false;st.running=false;st.sel=null;render()};$("plAgain").addEventListener("click",again);$("plAgainTop").addEventListener("click",again);
$("plFlip").addEventListener("click",()=>{st.flip=!st.flip;render()});
const pop=$("plPop");$("plGear").addEventListener("click",e=>{e.stopPropagation();pop.classList.toggle("on")});document.addEventListener("pointerdown",e=>{if(!e.target.closest("#plPop,#plGear"))pop.classList.remove("on")},true);
$("plSound").checked=SET.sound;$("plLegal").checked=SET.legal;
$("plSound").addEventListener("change",e=>{SET.sound=e.target.checked;saveSet()});
$("plLegal").addEventListener("change",e=>{SET.legal=e.target.checked;saveSet();render()});
$("plBadges").checked=SET.badges;$("plBadges").addEventListener("change",e=>{SET.badges=e.target.checked;saveSet();render()});$("plWood").checked=SET.wood;$("plB").classList.toggle("wood",SET.wood);$("plWood").addEventListener("change",e=>{SET.wood=e.target.checked;saveSet();$("plB").classList.toggle("wood",SET.wood)});$("plEvalOn").checked=SET.evalbar;$("plEvalOn").addEventListener("change",e=>{SET.evalbar=e.target.checked;saveSet();render()});
const rsB=$("plResign");
const modal=$("plModal");rsB.addEventListener("click",e=>{e.stopPropagation();if(!st.over)modal.classList.toggle("on")});
$("plMNo").addEventListener("click",()=>modal.classList.remove("on"));document.addEventListener("pointerdown",e=>{if(!e.target.closest(".pl-rsw"))modal.classList.remove("on")},true);
$("plMYes").addEventListener("click",()=>{modal.classList.remove("on");if(!st.over)finish("You resigned, engine wins")});
$("plAnalyze").addEventListener("click",analyze);

const fit=()=>{
  const pane=root.closest(".tabpane");if(!pane.classList.contains("on"))return;
  const L=root.querySelector(".pl-left"),side=$("plSide"),MV=$("plMoves"),mst=MV.scrollTop;L.style.width="";side.style.height="";
  const narrow=innerWidth<=900;
  const wrap=document.querySelector(".wrap"),pb=parseFloat(getComputedStyle(wrap).paddingBottom)||0;
  const top=L.getBoundingClientRect().top+scrollY;
  const avail=root.clientWidth-(narrow?0:300+12+25)-38;
  let w=Math.floor(Math.min(avail,narrow?avail:innerHeight-top-pb-4));
  w=Math.max(240,w);L.style.width=w+38+"px";requestAnimationFrame(()=>{const bw=B.getBoundingClientRect().width;B.style.setProperty("--bs",Math.round(bw/8*.42)+"px")});
  if(!narrow&&st.phase!=="setup"){side.style.height=L.getBoundingClientRect().height+"px";const o=document.documentElement.scrollHeight-innerHeight;if(o>0){L.style.width=Math.max(240,w-o)+38+"px";side.style.height=L.getBoundingClientRect().height+"px"}}
  MV.scrollTop=mst;
};
addEventListener("resize",fit);
document.querySelectorAll(".tabs button").forEach(b=>b.addEventListener("click",()=>requestAnimationFrame(fit)));
const origRender=render;
render=function(){origRender();requestAnimationFrame(fit)};
const [base]=TCS[st.tc];st.clock={w:base*1000,b:base*1000};
render();
})();

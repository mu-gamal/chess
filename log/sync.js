(()=>{
const KEYS=["cc_games","cc_games_del","cc_daily","cc_play","cc_theme"];
const FILE="chess-crack-sync.json";
const LS=localStorage,set0=Storage.prototype.setItem,get=k=>LS.getItem(k),put=(k,v)=>set0.call(LS,k,v);
const tok=()=>get("cc_sync_token"),meta=()=>{try{return JSON.parse(get("cc_sync_meta")||"{}")}catch(e){return {}}};
let applying=false,timer=null,busy=false,again=false,state="off",last=0,errMsg="";
Storage.prototype.setItem=function(k,v){set0.call(this,k,v);if(this===LS&&!applying&&KEYS.includes(k)){const m=meta();m[k]=Date.now();put("cc_sync_meta",JSON.stringify(m));schedule()}};
const api=async(path,opt={})=>{const r=await fetch("https://api.github.com"+path,{...opt,headers:{"Authorization":"Bearer "+tok(),"Accept":"application/vnd.github+json",...(opt.body?{"Content-Type":"application/json"}:{})}});if(!r.ok)throw new Error(r.status===401?"Invalid token":r.status===403?"Token has no Gists permission":"GitHub error "+r.status);return r.json()};
const J=s=>{try{return JSON.parse(s)}catch(e){return null}};
function local(){const data={},m=meta();KEYS.forEach(k=>{const v=get(k);if(v!==null)data[k]=v});return {data,ts:m}}
function merge(a,b){
  const out={data:{},ts:{}};
  const del=[...new Set([...(J(a.data.cc_games_del)||[]),...(J(b.data.cc_games_del)||[])])];
  const g=new Map();[...(J(b.data.cc_games)||[]),...(J(a.data.cc_games)||[])].forEach(x=>{const o=g.get(x.id);if(!o||(!o.anal&&x.anal))g.set(x.id,x)});
  const games=[...g.values()].filter(x=>!del.includes(x.id)).sort((x,y)=>y.id-x.id);
  if(games.length||a.data.cc_games!==undefined||b.data.cc_games!==undefined)out.data.cc_games=JSON.stringify(games);
  if(del.length)out.data.cc_games_del=JSON.stringify(del);
  ["cc_games","cc_games_del"].forEach(k=>{out.ts[k]=Math.max(a.ts[k]||0,b.ts[k]||0)});
  KEYS.slice(2).forEach(k=>{const ta=a.ts[k]||0,tb=b.ts[k]||0,src=a.data[k]===undefined?b:b.data[k]===undefined?a:ta>=tb?a:b;if(src.data[k]!==undefined){out.data[k]=src.data[k];out.ts[k]=Math.max(ta,tb)}});
  return out;
}
const same=(x,y)=>KEYS.every(k=>(x.data[k]??null)===(y.data[k]??null));
async function findGist(){
  let id=get("cc_sync_gist");if(id)return id;
  for(let p=1;p<=10;p++){const l=await api(`/gists?per_page=100&page=${p}`);const f=l.find(x=>x.files&&x.files[FILE]);if(f){put("cc_sync_gist",f.id);return f.id}if(l.length<100)break}
  const c=await api("/gists",{method:"POST",body:JSON.stringify({description:"Chess Crack sync",public:false,files:{[FILE]:{content:JSON.stringify({v:1,...local()})}}})});
  put("cc_sync_gist",c.id);return c.id;
}
async function remote(id){
  const g=await api("/gists/"+id),f=g.files[FILE];if(!f)return {data:{},ts:{}};
  const txt=f.truncated?await (await fetch(f.raw_url)).text():f.content;const o=J(txt)||{};return {data:o.data||{},ts:o.ts||{}};
}
function applyLocal(m){applying=true;try{KEYS.forEach(k=>{if(m.data[k]!==undefined)put(k,m.data[k])});put("cc_sync_meta",JSON.stringify({...meta(),...m.ts}))}finally{applying=false}}
async function sync(first){
  if(!tok())return;if(busy){again=true;return}busy=true;setState("busy");
  try{
    let id=await findGist(),r;
    try{r=await remote(id)}catch(e){if(/404/.test(e.message)){LS.removeItem("cc_sync_gist");id=await findGist();r=await remote(id)}else throw e}
    const l=local(),m=merge(l,r);
    if(!same(m,r))await api("/gists/"+id,{method:"PATCH",body:JSON.stringify({files:{[FILE]:{content:JSON.stringify({v:1,...m})}}})});
    if(!same(m,l)){applyLocal(m);if(first&&!sessionStorage.getItem("cc_sync_rl")){sessionStorage.setItem("cc_sync_rl","1");location.reload();return}}
    sessionStorage.removeItem("cc_sync_rl");last=Date.now();errMsg="";setState("ok");
  }catch(e){errMsg=e.message||"Sync failed";setState("err")}
  finally{busy=false;if(again){again=false;schedule()}}
}
function schedule(){if(!tok())return;clearTimeout(timer);setState("busy");timer=setTimeout(()=>sync(false),2000)}
const t12=ts=>new Date(ts).toLocaleTimeString("en",{hour:"numeric",minute:"2-digit"});
function setState(s){state=s;const b=document.getElementById("syncBtn");if(!b)return;b.dataset.s=s;b.title=s==="ok"?"Synced "+t12(last):s==="busy"?"Syncing…":s==="err"?errMsg:"Sync off";renderPop()}
function renderPop(){
  const p=document.getElementById("syncPop");if(!p||!p.classList.contains("on"))return;
  if(!tok()){p.innerHTML=`<div class="sp-t">Sync</div><div class="sp-s">Paste a GitHub token with Gists read & write</div><input class="pl-in" id="spTok" type="password" placeholder="github_pat_…" spellcheck="false"><div class="pl-err" id="spErr"></div><button class="pl-btn main" id="spGo">Connect</button>`;return}
  p.innerHTML=`<div class="sp-t">Sync</div><div class="sp-s ${state}">${state==="ok"?"Synced at "+t12(last):state==="busy"?"Syncing…":state==="err"?errMsg:""}</div><div class="sp-row"><button class="pl-btn" id="spNow">Sync now</button><button class="pl-btn" id="spOff">Disconnect</button></div>`;
}
function ui(){
  const th=document.getElementById("themeBtn");if(!th)return;
  const w=document.createElement("div");w.className="syncw";
  w.innerHTML=`<button class="themebtn syncbtn" id="syncBtn" data-s="off" title="Sync off"><svg viewBox="0 0 24 24"><path d="M6.5 19a4.5 4.5 0 0 1-.4-9A6 6 0 0 1 17.7 8.6 5.2 5.2 0 0 1 17.5 19z"/></svg><i></i></button><div class="sync-pop" id="syncPop"></div>`;
  th.parentNode.insertBefore(w,th);
  const p=w.querySelector("#syncPop");
  w.querySelector("#syncBtn").addEventListener("click",e=>{e.stopPropagation();p.classList.toggle("on");renderPop()});
  document.addEventListener("pointerdown",e=>{if(!e.target.closest(".syncw"))p.classList.remove("on")},true);
  p.addEventListener("click",async e=>{
    const id=e.target.id;
    if(id==="spGo"){const v=document.getElementById("spTok").value.trim();if(!v)return;put("cc_sync_token",v);LS.removeItem("cc_sync_gist");
      try{await api("/user")}catch(er){LS.removeItem("cc_sync_token");document.getElementById("spErr").textContent=er.message;return}
      renderPop();sync(true)}
    else if(id==="spNow")sync(false);
    else if(id==="spOff"){LS.removeItem("cc_sync_token");LS.removeItem("cc_sync_gist");setState("off");renderPop()}
  });
  setState(tok()?"busy":"off");
}
document.readyState==="loading"?document.addEventListener("DOMContentLoaded",()=>{ui();sync(true)}):(ui(),sync(true));
})();

// ======= UI helper =======
const $ = (id) => document.getElementById(id);

// ======= Modal controls =======
function openLogin(){ $('modalLogin').style.display = 'flex'; }
function closeLogin(){ $('modalLogin').style.display = 'none'; }
function openSignup(){ $('modalSignup').style.display = 'flex'; }
function closeSignup(){ $('modalSignup').style.display = 'none'; }

// ======= Simple swap & clear =======
function swapPlaces(){
  const s = $('start'), e = $('end');
  [s.value, e.value] = [e.value, s.value];
  s.focus();
}

function clearFields(){
  $('start').value = '';
  $('end').value = '';
  $('busBody').innerHTML = `<div class="placeholder">Awaiting backend data...</div>`;
  $('metroBody').innerHTML = `<div class="placeholder">Backend will inject metro data here.</div>`;
  $('start').focus();
}

// ======= Login / Signup (frontend-only) =======
function loginUser(){
  const email = $('loginEmail').value.trim();
  const pass = $('loginPass').value.trim();
  if(!email || !pass){
    alert('Please enter email and password');
    return;
  }
  setLoggedIn(email);
  closeLogin();
}

function signupUser(){
  const name = $('signName').value.trim();
  const email = $('signEmail').value.trim();
  const pass = $('signPass').value.trim();

  if(!name || !email || !pass){
    alert('Please fill all fields');
    return;
  }
  setLoggedIn(email, name);
  closeSignup();
}

function setLoggedIn(email, name){
  $('navActions').innerHTML = `
    <div class="user-pill">
      <i class="fa-solid fa-user"></i>
      <span class="user-email">${name ? name : email}</span>
      <button class="btn ghost" onclick="logout()" style="margin-left:12px">
        <i class="fa-solid fa-right-from-bracket"></i> Logout
      </button>
    </div>
  `;
}

function logout(){
  $('navActions').innerHTML = `
    <button class="btn ghost" onclick="openLogin()">
      <i class="fa-solid fa-right-to-bracket"></i> Login
    </button>
    <button class="btn primary" onclick="openSignup()">
      <i class="fa-solid fa-user-plus"></i> Sign Up
    </button>
  `;
}

// ======= Skeleton Helpers =======
function showLoadingSkeletons(){
  const ids = ['busBody','metroBody','trafficBody','altBody','etaBody'];
  ids.forEach(id=>{
    const el = $(id);
    if(el){
      el.innerHTML = `
        <div style="width:100%;display:flex;flex-direction:column;gap:10px">
          <div class="skeleton" style="width:60%"></div>
          <div class="skeleton" style="width:90%"></div>
          <div class="skeleton" style="width:40%"></div>
        </div>
      `;
    }
  });
}

function hideSkeletonsKeepPlaceholders(){
  const map = {
    metroBody: '<div class="placeholder">Backend will inject metro data here.</div>',
    trafficBody: '<div class="placeholder">Traffic is - MODERATE</div>',
    altBody: '<div class="placeholder">book taxi from app ola, uber ,rapido</div>',
    etaBody: '<div class="placeholder">#discount via book using official DMRC , DTC WEBSITE</div>',
  };
  Object.keys(map).forEach(k => {
    if($(k)) $(k).innerHTML = map[k];
  });
}

// ======= BUS SEARCH LOGIC (UNCHANGED) =======
async function searchRoute(){
  const start = $('start').value.trim();
  const end = $('end').value.trim();

  if(!start || !end){
    alert('Enter both start and destination');
    return;
  }

  document.querySelector(".results-wrap").style.display = "block";
  showLoadingSkeletons();

  try {
    const response = await fetch("http://127.0.0.1:5000/bus-route", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ start, end })
    });

    const data = await response.json();

    if (data.error) {
      $('busBody').innerHTML = `<div class="placeholder">${data.error}</div>`;
    } else {
      $('busBody').innerHTML = `
        <pre style="white-space: pre-wrap; font-size:14px; line-height:1.6">
${data.raw}
        </pre>
      `;
    }

  } catch (err) {
    $('busBody').innerHTML =
      `<div class="placeholder">Flask server not running</div>`;
  }

  hideSkeletonsKeepPlaceholders();
}

// ======= METRO SEARCH LOGIC (Refactored, flash removed) =======
async function searchMetroRoute() {
  const start = $('start').value.trim();
  const end = $('end').value.trim();

  if (!start || !end) {
    $('metroBody').innerHTML = `<div class="placeholder">Enter start and destination</div>`;
    return;
  }

  // Skeleton already shown by searchAll, so don't re-render here

  try {
    const response = await fetch("http://127.0.0.1:5000/metro-route", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ start, end })
    });

    const data = await response.json();

    if (data.error) {
      $('metroBody').innerHTML = `<div class="placeholder">${data.error}</div>`;
    } else {
      let html = '<pre style="white-space: pre-wrap; font-size:14px; line-height:1.6">';
      data.lines.forEach(line => html += line + '\n');
      html += '</pre>';

      $('metroBody').innerHTML = html;
    }

  } catch (err) {
    $('metroBody').innerHTML =
      `<div class="placeholder">Flask server not running</div>`;
  }
}

// ======= MAIN SEARCH BUTTON =======
async function searchAll() {
  // Show skeletons once
  showLoadingSkeletons();

  // Run bus and metro searches in parallel (or await if sequence needed)
  await searchRoute();
  await searchMetroRoute();
}

// ===== INIT =====
document.addEventListener('DOMContentLoaded', () => {
  hideSkeletonsKeepPlaceholders();
  document.querySelector(".results-wrap").style.display = "none";

  // Prevent form reload if search button inside a form
  const form = document.getElementById("searchForm");
  if(form){
    form.addEventListener("submit", function(e){
      e.preventDefault();
      searchAll();
    });
  }
});

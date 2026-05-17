/* ============================================
   NOVABANC — APP LOGIC
   Calls bank.c via server.py API.
   All business logic lives in the C program.
   ============================================ */

const API = "http://localhost:8080/api";

let currentUser = null; // { name, acc_no, balance, password_cache }

/* ===================== API HELPER ===================== */

async function api(endpoint, body) {
  try {
    const res = await fetch(API + endpoint, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body)
    });
    return await res.json();
  } catch (e) {
    return { ok: false, msg: "Cannot connect to server. Is server.py running?" };
  }
}

/* ===================== PAGE NAVIGATION ===================== */

function showPage(pageId) {
  document.querySelectorAll(".page").forEach(p => p.classList.remove("active"));
  document.getElementById(pageId).classList.add("active");
}

function showDash(tabId) {
  document.querySelectorAll(".dash-tab").forEach(t => t.classList.remove("active"));
  document.getElementById(tabId).classList.add("active");
  document.querySelectorAll(".nav-item[data-tab]").forEach(n => {
    n.classList.toggle("active", n.dataset.tab === tabId);
  });
  if (tabId === "dash-home") loadDashHome();
  if (tabId === "dash-history") loadHistory();
  if (tabId === "dash-update") prefillUpdate();
}

/* ===================== TOAST ===================== */

let toastTimer;
function showToast(msg, type = "info") {
  const t = document.getElementById("toast");
  t.textContent = msg;
  t.className = "toast show " + type;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => { t.className = "toast"; }, 3500);
}

/* ===================== LOGIN ===================== */

async function doLogin() {
  const acc_no = parseInt(document.getElementById("login-acc").value);
  const password = document.getElementById("login-pass").value.trim();

  if (!acc_no || !password) {
    showToast("Please fill in all fields.", "error"); return;
  }

  const data = await api("/login", { acc_no, password });

  if (data.ok) {
    currentUser = {
      name: data.name,
      acc_no: data.acc_no,
      balance: data.balance,
      password_cache: password   // kept in RAM only, never saved to disk
    };
    document.getElementById("login-acc").value = "";
    document.getElementById("login-pass").value = "";
    showPage("page-dashboard");
    showDash("dash-home");
    showToast(`Welcome back, ${data.name.replace(/_/g, " ").split(" ")[0]}!`, "success");
  } else {
    showToast(data.msg, "error");
  }
}

function doLogout() {
  currentUser = null;
  showPage("page-login");
  showToast("Signed out successfully.");
}

/* ===================== CREATE ACCOUNT ===================== */

async function createAccount() {
  const name = document.getElementById("create-name").value.trim().replace(/ /g, "_");
  const acc_no = parseInt(document.getElementById("create-acc").value);
  const password = document.getElementById("create-pass").value.trim();
  const initial_deposit = parseFloat(document.getElementById("create-deposit").value);

  if (!name || !acc_no || !password || !initial_deposit) {
    showToast("All fields are required.", "error"); return;
  }

  if (initial_deposit < 500) {
    showToast("Minimum initial deposit is ₹500.", "error"); return;
  }

  const data = await api("/create", { name, acc_no, password, initial_deposit });

  if (data.ok) {
    document.getElementById("create-name").value = "";
    document.getElementById("create-acc").value = "";
    document.getElementById("create-pass").value = "";
    document.getElementById("create-deposit").value = "";
    showToast("Account created successfully! Opening balance: ₹" + initial_deposit.toFixed(2), "success");
    showPage("page-login");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== DASHBOARD HOME ===================== */

async function loadDashHome() {
  if (!currentUser) return;
  document.getElementById("dash-name").textContent = currentUser.name.replace(/_/g, " ");
  document.getElementById("dash-accno").textContent = currentUser.acc_no;
  document.getElementById("dash-balance").textContent = formatCurrency(currentUser.balance);
  loadRecentTransactions();
}

async function loadRecentTransactions() {
  const data = await api("/history", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache
  });
  const list = document.getElementById("recent-list");
  list.innerHTML = renderTxnList(data.ok ? data.transactions.slice(0, 5) : []);
}

async function loadHistory() {
  if (!currentUser) return;
  const data = await api("/history", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache
  });
  document.getElementById("history-list").innerHTML =
    renderTxnList(data.ok ? data.transactions : []);
}

function renderTxnList(txns) {
  if (!txns || txns.length === 0)
    return `<div class="empty-state">No transactions yet.</div>`;

  return txns.map(line => {
    const isDeposit  = line.includes("Deposited");
    const isTransfer = line.includes("Transferred");
    const cls    = isDeposit ? "dep" : isTransfer ? "trans" : "with";
    const icon   = isDeposit ? "↓"  : isTransfer ? "⇄"     : "↑";
    const sign   = isDeposit ? "+"  : "-";
    const action = isDeposit ? "Deposited" : isTransfer ? "Transferred" : "Withdrawn";
    const amtMatch = line.match(/Amount: ([\d.]+)/);
    const amount = amtMatch ? parseFloat(amtMatch[1]) : 0;
    const parts  = line.split("|");
    const time   = parts[3] ? parts[3].trim() : "";

    return `
      <div class="txn-item">
        <div class="txn-left">
          <div class="txn-icon ${cls}">${icon}</div>
          <div>
            <div class="txn-type">${action}</div>
            <div class="txn-time">${time}</div>
          </div>
        </div>
        <div class="txn-amount ${cls}">${sign}${formatCurrency(amount)}</div>
      </div>`;
  }).join("");
}

/* ===================== DEPOSIT ===================== */

async function depositMoney() {
  const amount = parseFloat(document.getElementById("dep-amount").value);
  if (!amount || amount <= 0) { showToast("Enter a valid amount.", "error"); return; }

  const data = await api("/deposit", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache,
    amount
  });

  if (data.ok) {
    currentUser.balance = data.balance;
    document.getElementById("dep-amount").value = "";
    document.getElementById("dash-balance").textContent = formatCurrency(data.balance);
    showToast(`Deposited ${formatCurrency(amount)} successfully!`, "success");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== WITHDRAW ===================== */

async function withdrawMoney() {
  const amount = parseFloat(document.getElementById("with-amount").value);
  if (!amount || amount <= 0) { showToast("Enter a valid amount.", "error"); return; }

  const data = await api("/withdraw", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache,
    amount
  });

  if (data.ok) {
    currentUser.balance = data.balance;
    document.getElementById("with-amount").value = "";
    document.getElementById("dash-balance").textContent = formatCurrency(data.balance);
    showToast(`Withdrawn ${formatCurrency(amount)} successfully!`, "success");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== TRANSFER ===================== */

async function transferMoney() {
  const to_acc = parseInt(document.getElementById("trans-to").value);
  const amount = parseFloat(document.getElementById("trans-amount").value);

  if (!to_acc || !amount || amount <= 0) { showToast("Fill all fields.", "error"); return; }
  if (to_acc === currentUser.acc_no) { showToast("Cannot transfer to yourself.", "error"); return; }

  const data = await api("/transfer", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache,
    to_acc, amount
  });

  if (data.ok) {
    currentUser.balance = data.balance;
    document.getElementById("trans-to").value = "";
    document.getElementById("trans-amount").value = "";
    document.getElementById("dash-balance").textContent = formatCurrency(data.balance);
    showToast(`Transferred ${formatCurrency(amount)} to Account #${to_acc}!`, "success");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== UPDATE ACCOUNT ===================== */

function prefillUpdate() {
  if (currentUser)
    document.getElementById("upd-name").value = currentUser.name.replace(/_/g, " ");
}

async function updateAccount() {
  const new_name = document.getElementById("upd-name").value.trim().replace(/ /g, "_");
  const new_password = document.getElementById("upd-pass").value.trim();

  if (!new_name || !new_password) { showToast("Both fields required.", "error"); return; }

  const data = await api("/update", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache,
    new_name, new_password
  });

  if (data.ok) {
    currentUser.name = new_name;
    currentUser.password_cache = new_password;
    document.getElementById("dash-name").textContent = new_name.replace(/_/g, " ");
    document.getElementById("upd-pass").value = "";
    showToast("Account updated successfully!", "success");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== DELETE ACCOUNT ===================== */

function confirmDelete() {
  openModal(
    "Delete Account",
    `Permanently delete Account #${currentUser.acc_no}? This cannot be undone.`,
    deleteAccount
  );
}

async function deleteAccount() {
  const data = await api("/delete", {
    acc_no: currentUser.acc_no,
    password: currentUser.password_cache
  });
  closeModal();
  if (data.ok) {
    currentUser = null;
    showPage("page-login");
    showToast("Account deleted successfully.", "success");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== ADMIN ===================== */

async function adminLogin() {
  const admin_password = document.getElementById("admin-pass").value;
  const data = await api("/admin", { admin_password });

  if (data.ok) {
    document.getElementById("admin-pass").value = "";
    const grid = document.getElementById("admin-accounts");
    if (!data.accounts || data.accounts.length === 0) {
      grid.innerHTML = `<div class="empty-state">No accounts found.</div>`;
    } else {
      grid.innerHTML = data.accounts.map(acc => `
        <div class="account-card">
          <div class="acc-card-name">${escapeHtml(acc.name.replace(/_/g, " "))}</div>
          <div class="acc-card-no">Account #${acc.acc_no}</div>
          <div class="acc-card-bal-label">Balance</div>
          <div class="acc-card-bal">${formatCurrency(acc.balance)}</div>
        </div>`).join("");
    }
    showPage("page-admin");
  } else {
    showToast(data.msg, "error");
  }
}

/* ===================== MODAL ===================== */

function openModal(title, msg, onConfirm) {
  document.getElementById("modal-title").textContent = title;
  document.getElementById("modal-msg").textContent = msg;
  document.getElementById("modal-confirm").onclick = onConfirm;
  document.getElementById("modal").classList.remove("hidden");
}

function closeModal() {
  document.getElementById("modal").classList.add("hidden");
}

/* ===================== UTILS ===================== */

function formatCurrency(amount) {
  return "₹" + parseFloat(amount || 0).toLocaleString("en-IN", {
    minimumFractionDigits: 2, maximumFractionDigits: 2
  });
}

function escapeHtml(str) {
  return String(str).replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;");
}

/* ===================== KEYBOARD ===================== */

document.addEventListener("keydown", e => {
  if (e.key === "Enter") {
    const p = document.querySelector(".page.active");
    if (!p) return;
    if (p.id === "page-login") doLogin();
    else if (p.id === "page-create") createAccount();
    else if (p.id === "page-admin-login") adminLogin();
  }
  if (e.key === "Escape") closeModal();
});

document.getElementById("modal").addEventListener("click", e => {
  if (e.target === document.getElementById("modal")) closeModal();
});
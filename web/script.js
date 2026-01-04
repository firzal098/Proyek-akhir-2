const apiUrl = 'api.php';

async function apiGet(type=''){
    const url = type ? `${apiUrl}?type=${type}` : apiUrl;
    const res = await fetch(url);
    return res.json();
}

async function apiPost(action, payload={}){
    const body = Object.assign({action}, payload);
    const res = await fetch(apiUrl, {
        method: 'POST',
        headers: {'Content-Type':'application/json'},
        body: JSON.stringify(body)
    });
    return res.json().catch(()=>({success:false}));
}

// --- UI helpers ---
function el(id){ return document.getElementById(id); }
function show(id){ el(id).classList.remove('hidden'); }
function hide(id){ el(id).classList.add('hidden'); }

function saveUserToStorage(u){ localStorage.setItem('app_user', JSON.stringify(u)); }
function loadUserFromStorage(){ const s = localStorage.getItem('app_user'); return s?JSON.parse(s):null; }
function clearUserStorage(){ localStorage.removeItem('app_user'); }

// --- Auth flows ---
document.getElementById('loginForm').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const id = el('loginId').value.trim();
    const pw = el('loginPassword').value.trim();
    const r = await apiPost('login',{id, password: pw});
    if (r.success){
        if (r.role === 'admin'){
            saveUserToStorage({role:'admin'});
            renderApp();
        } else {
            saveUserToStorage({role:'customer', customer: r.customer});
            renderApp();
        }
    } else alert(r.message || 'Login gagal');
});

document.getElementById('registerForm').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const id = el('regId').value.trim();
    const nama = el('regNama').value.trim();
    const telepon = el('regTelp').value.trim();
    const email = el('regEmail').value.trim();
    const pw = el('regPassword').value.trim();
    const r = await apiPost('register',{id,nama,telepon,email,password:pw});
    if (r.success) { alert('Registrasi berhasil. Silakan login.');
        el('regId').value=''; el('regNama').value=''; el('regPassword').value='';
    } else alert(r.message || 'Gagal register');
});

el('logoutBtn').addEventListener('click', ()=>{ clearUserStorage(); renderApp(); });

// --- Admin actions ---
async function loadCustomers(){
    const data = await apiGet('customers');
    const container = el('customersList');
    container.innerHTML='';
    if (!data.length) container.innerText='(Belum ada pelanggan)';
    data.forEach(c=>{
        const d = document.createElement('div');
        d.className='item';
        d.innerHTML = `<b>${c.nama}</b> (ID: ${c.id}) - Saldo: Rp ${c.saldo}`;
        container.appendChild(d);
    });
}

document.getElementById('giveBalanceForm').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const id = el('gbCustomerId').value.trim();
    const amt = parseFloat(el('gbAmount').value);
    const r = await apiPost('give_balance',{customerId:id, amount: amt});
    if (r.success){ alert('Saldo diberikan.'); loadCustomers(); } else alert('Gagal');
});

// --- Vendor management ---
document.getElementById('formVendor').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const nama = el('namaVendor').value.trim();
    const pu = el('produkUtama').value.trim();
    const kat = el('kategori').value.trim();
    const r = await apiPost('add_vendor',{nama, produkUtama:pu, kategori:kat});
    if (r.success){ el('namaVendor').value=''; el('produkUtama').value=''; el('kategori').value=''; loadVendorsForAdmin(); loadVendorsForCustomer(); }
});

async function loadVendorsForAdmin(){
    const vendors = await apiGet('vendors');
    const c = el('vendorListAdmin'); c.innerHTML='';
    if (!vendors.length) c.innerText='(Belum ada vendor)';
    vendors.forEach(v=>{
        const card = document.createElement('div'); card.className='vendorCard';
        card.innerHTML = `<b>${v.nama}</b> (ID:${v.id})<br>Kat: ${v.kategori}<br>Produk utama: ${v.produkUtama}`;
        const btn = document.createElement('button'); btn.textContent='Tambah Produk'; btn.className='btn-primary small';
        btn.onclick = ()=>{ openProductModal(v.id); };
        card.appendChild(btn);
        // list products
        const ul = document.createElement('div'); ul.className='prodList';
        if (!v.daftarProduk || !v.daftarProduk.length) ul.innerText='(Belum ada produk)';
        else v.daftarProduk.forEach(p=>{ const pdiv=document.createElement('div'); pdiv.innerText=`${p.nama} - Rp${p.harga} | Stok:${p.stok}`; ul.appendChild(pdiv); });
        card.appendChild(ul);
        c.appendChild(card);
    });
}

function openProductModal(vendorId){ el('currentVendorId').value = vendorId; show('productModal'); }
el('closeModal').addEventListener('click', ()=>{ hide('productModal'); });

document.getElementById('formProduct').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const vendorId = parseInt(el('currentVendorId').value);
    const nama = el('namaProduk').value.trim();
    const harga = parseFloat(el('hargaProduk').value);
    const stok = parseInt(el('stokProduk').value);
    const r = await apiPost('add_product',{vendorId, nama, harga, stok});
    if (r.success){ hide('productModal'); el('namaProduk').value=''; el('hargaProduk').value=''; el('stokProduk').value=''; loadVendorsForAdmin(); loadVendorsForCustomer(); }
});

// --- Customer views ---
async function loadVendorsForCustomer(){
    const vendors = await apiGet('vendors');
    const c = el('vendorListCustomer'); c.innerHTML='';
    if (!vendors.length) c.innerText='(Belum ada vendor)';
    vendors.forEach(v=>{
        const card = document.createElement('div'); card.className='vendorCard';
        card.innerHTML = `<b>${v.nama}</b> (ID:${v.id})<br>Kat: ${v.kategori}`;
        if (v.daftarProduk && v.daftarProduk.length){
            v.daftarProduk.forEach(p=>{
                const pdiv = document.createElement('div');
                pdiv.innerHTML = `${p.nama} - Rp${p.harga} | Stok:${p.stok} `;
                const btn = document.createElement('button'); btn.className='btn-primary small'; btn.textContent='Beli';
                btn.onclick = async ()=>{
                    const qty = parseInt(prompt('Jumlah yang ingin dibeli', '1')) || 1;
                    const user = loadUserFromStorage();
                    if (!user || user.role !== 'customer') return alert('Silakan login sebagai pelanggan.');
                    const res = await apiPost('buy_product',{customerId:user.customer.id, vendorId:v.id, productName:p.nama, qty});
                    if (res.success){ alert('Pembelian berhasil. Sisa saldo: Rp '+res.saldo); updateCustomerInfo(); loadVendorsForCustomer(); loadVendorsForAdmin(); }
                    else alert(res.message || 'Gagal beli');
                };
                pdiv.appendChild(btn);
                card.appendChild(pdiv);
            });
        }
        c.appendChild(card);
    });
}

// --- Parkir UI ---
function renderMap(parkir){
    const mapArea = el('mapArea'); mapArea.innerHTML='';
    const rows = ['A','B','C','D','E'];
    rows.forEach(r=>{
        const rowDiv = document.createElement('div'); rowDiv.className='mapRow';
        for (let j=1;j<=5;j++){
            const slot = r + j;
            const taken = parkir.aktif.some(t=>t.slot===slot && t.status==='AKTIF');
            const btn = document.createElement('div'); btn.className = 'slot ' + (taken? 'taken':'free'); btn.textContent = slot;
            rowDiv.appendChild(btn);
        }
        mapArea.appendChild(rowDiv);
    });
}

async function loadParkirAndTickets(){
    const parkir = await apiGet('parkir');
    renderMap(parkir);
    const at = el('activeTickets'); at.innerHTML='';
    if (!parkir.aktif.length) at.innerText='(Tidak ada kendaraan aktif)';
    parkir.aktif.forEach(t=>{ const d=document.createElement('div'); d.innerText=`${t.idTiket} | ${t.nomorPolisi} | Slot:${t.slot}`; at.appendChild(d); });
}

document.getElementById('checkinForm').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const slot = el('checkinSlot').value.trim();
    const plat = el('checkinPlat').value.trim();
    const user = loadUserFromStorage();
    if (!user || user.role !== 'customer') return alert('Silakan login sebagai pelanggan.');
    const r = await apiPost('checkin',{customerId:user.customer.id, plat, slot});
    if (r.success){ alert('Check-in berhasil. ID tiket: '+r.tiket.idTiket); loadParkirAndTickets(); } else alert(r.message||'Gagal check-in');
});

document.getElementById('checkoutForm').addEventListener('submit', async (e)=>{
    e.preventDefault();
    const idT = el('checkoutTiket').value.trim();
    const r = await apiPost('checkout',{idTiket:idT});
    if (r.success){ alert('Check-out berhasil. Biaya Rp '+r.tiket.biaya); loadParkirAndTickets(); } else alert(r.message || 'Gagal checkout');
});

async function updateCustomerInfo(){
    const user = loadUserFromStorage();
    if (!user || user.role !== 'customer') return;
    // re-load customer list and update the customer object (cheap approach)
    const customers = await apiGet('customers');
    const me = customers.find(c=>c.id===user.customer.id);
    if (me){ user.customer = me; saveUserToStorage(user); el('custSaldo').innerText = me.saldo; }
}

// --- Render app based on role ---
async function renderApp(){
    const user = loadUserFromStorage();
    if (!user){ show('authSection'); hide('adminPanel'); hide('customerPanel'); return; }
    hide('authSection');
    if (user.role === 'admin'){
        show('adminPanel'); hide('customerPanel');
        await loadCustomers(); await loadVendorsForAdmin();
    } else if (user.role === 'customer'){
        hide('adminPanel'); show('customerPanel');
        el('custName').innerText = user.customer.nama;
        el('custId').innerText = user.customer.id;
        el('custSaldo').innerText = user.customer.saldo;
        await loadVendorsForCustomer(); await loadParkirAndTickets();
    }
}

// Init
renderApp();
document.addEventListener('DOMContentLoaded', () => {
    loadVendors();

    // Setup Modal
    const modal = document.getElementById('productModal');
    const span = document.getElementsByClassName("close")[0];
    span.onclick = function() { modal.style.display = "none"; }
    window.onclick = function(event) { if (event.target == modal) modal.style.display = "none"; }

    // Handle Tambah Vendor
    document.getElementById('formVendor').addEventListener('submit', async (e) => {
        e.preventDefault();
        const data = {
            action: 'add_vendor',
            nama: document.getElementById('namaVendor').value,
            produkUtama: document.getElementById('produkUtama').value,
            kategori: document.getElementById('kategori').value
        };
        
        await fetch('api.php', { method: 'POST', body: JSON.stringify(data) });
        document.getElementById('formVendor').reset();
        loadVendors();
    });

    // Handle Tambah Produk
    document.getElementById('formProduct').addEventListener('submit', async (e) => {
        e.preventDefault();
        const data = {
            action: 'add_product',
            vendorId: document.getElementById('currentVendorId').value,
            nama: document.getElementById('namaProduk').value,
            harga: document.getElementById('hargaProduk').value,
            stok: document.getElementById('stokProduk').value
        };

        await fetch('api.php', { method: 'POST', body: JSON.stringify(data) });
        document.getElementById('formProduct').reset();
        modal.style.display = "none";
        loadVendors();
    });
});

async function loadVendors() {
    const res = await fetch('api.php');
    const vendors = await res.json();
    const container = document.getElementById('vendorList');
    container.innerHTML = '';

    if (vendors.length === 0) {
        container.innerHTML = '<p style="text-align:center; color:#777;">Belum ada vendor terdaftar.</p>';
        return;
    }

    vendors.forEach(v => {
        const div = document.createElement('div');
        div.className = 'card';
        
        let productsHtml = '<p style="color:#888; font-style:italic;">Belum ada produk</p>';
        if (v.daftarProduk && v.daftarProduk.length > 0) {
            productsHtml = '<ul>' + v.daftarProduk.map(p => 
                `<li><span>${p.nama} (Stok: ${p.stok})</span> <strong>Rp${parseInt(p.harga).toLocaleString()}</strong></li>`
            ).join('') + '</ul>';
        }

        div.innerHTML = `
            <div class="vendor-header">
                <h3>${v.nama} <span class="badge">${v.kategori}</span></h3>
                <small>ID: ${v.id}</small>
            </div>
            <p><strong>Produk Utama:</strong> ${v.produkUtama}</p>
            <div class="product-list">
                ${productsHtml}
            </div>
            <button class="btn-add-prod" onclick="document.getElementById('currentVendorId').value='${v.id}'; document.getElementById('productModal').style.display='block';">Tambah Produk</button>
        `;
        container.appendChild(div);
    });
}
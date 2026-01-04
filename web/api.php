<?php
header('Content-Type: application/json');
$file = __DIR__ . '/data.json';

// Initialize data file structure if missing
if (!file_exists($file)) {
    $init = [
        'customers' => [],
        'vendors' => [],
        'parkir' => [
            'aktif' => [],
            'riwayat' => [],
            'poin' => []
        ]
    ];
    file_put_contents($file, json_encode($init, JSON_PRETTY_PRINT));
}

function getData() {
    global $file;
    $content = file_get_contents($file);
    return json_decode($content, true) ?? [];
}

function saveData($data) {
    global $file;
    file_put_contents($file, json_encode($data, JSON_PRETTY_PRINT));
}

// Helpers
function findCustomer(&$data, $id) {
    foreach ($data['customers'] as $i => $c) {
        if ($c['id'] == $id) return $i;
    }
    return -1;
}

function findVendorIndex(&$data, $id) {
    foreach ($data['vendors'] as $i => $v) {
        if ($v['id'] == $id) return $i;
    }
    return -1;
}

$method = $_SERVER['REQUEST_METHOD'];

// Simple router using action param for POST and query for GET
if ($method === 'GET') {
    $type = $_GET['type'] ?? '';
    $data = getData();
    if ($type === 'vendors') echo json_encode($data['vendors']);
    elseif ($type === 'customers') echo json_encode($data['customers']);
    elseif ($type === 'parkir') echo json_encode($data['parkir']);
    else echo json_encode($data);
    exit;
}

if ($method === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true) ?? [];
    $action = $input['action'] ?? '';
    $data = getData();

    // --- Pelanggan: register ---
    if ($action === 'register') {
        $id = $input['id'] ?? '';
        $nama = $input['nama'] ?? '';
        $telepon = $input['telepon'] ?? '';
        $email = $input['email'] ?? '';
        $password = $input['password'] ?? '';

        // check id uniqueness
        foreach ($data['customers'] as $c) {
            if ($c['id'] === $id) {
                http_response_code(400);
                echo json_encode(['success' => false, 'message' => 'ID sudah digunakan']);
                exit;
            }
        }

        $customer = [
            'id' => $id,
            'nama' => $nama,
            'telepon' => $telepon,
            'email' => $email,
            'password' => $password,
            'saldo' => 0.0
        ];
        $data['customers'][] = $customer;
        saveData($data);
        echo json_encode(['success' => true, 'customer' => $customer]);
        exit;
    }

    // --- Pelanggan: login ---
    if ($action === 'login') {
        $id = $input['id'] ?? '';
        $password = $input['password'] ?? '';
        if ($id === 'admin' && $password === 'admin123') {
            echo json_encode(['success' => true, 'role' => 'admin']); exit;
        }
        foreach ($data['customers'] as $c) {
            if ($c['id'] === $id && $c['password'] === $password) {
                echo json_encode(['success' => true, 'role' => 'customer', 'customer' => $c]); exit;
            }
        }
        http_response_code(401);
        echo json_encode(['success' => false, 'message' => 'ID atau password salah']);
        exit;
    }

    // --- Admin: beri saldo ---
    if ($action === 'give_balance') {
        $custId = $input['customerId'] ?? '';
        $amount = (float)($input['amount'] ?? 0);
        $idx = findCustomer($data, $custId);
        if ($idx < 0) { http_response_code(404); echo json_encode(['success'=>false]); exit; }
        $data['customers'][$idx]['saldo'] += $amount;
        saveData($data);
        echo json_encode(['success'=>true, 'customer'=>$data['customers'][$idx]]);
        exit;
    }

    // --- Vendor: add vendor ---
    if ($action === 'add_vendor') {
        $maxId = 0; foreach ($data['vendors'] as $v) if (isset($v['id']) && $v['id']>$maxId) $maxId = $v['id'];
        $newVendor = [
            'id' => $maxId+1,
            'nama' => $input['nama'] ?? 'Tanpa Nama',
            'produkUtama' => $input['produkUtama'] ?? '-',
            'kategori' => $input['kategori'] ?? 'Umum',
            'daftarProduk' => []
        ];
        $data['vendors'][] = $newVendor;
        saveData($data);
        echo json_encode(['success'=>true,'vendor'=>$newVendor]); exit;
    }

    // --- Vendor: add product ---
    if ($action === 'add_product') {
        $vendorId = $input['vendorId'] ?? 0;
        $i = findVendorIndex($data, $vendorId);
        if ($i < 0) { http_response_code(404); echo json_encode(['success'=>false]); exit; }
        $product = ['nama'=>$input['nama'],'harga'=> (float)$input['harga'],'stok'=> (int)$input['stok']];
        $data['vendors'][$i]['daftarProduk'][] = $product;
        saveData($data);
        echo json_encode(['success'=>true]); exit;
    }

    // --- Toko: buy product ---
    if ($action === 'buy_product') {
        $custId = $input['customerId'] ?? '';
        $vendorId = $input['vendorId'] ?? 0;
        $namaProduk = $input['productName'] ?? '';
        $qty = (int)($input['qty'] ?? 1);

        $ci = findCustomer($data, $custId);
        $vi = findVendorIndex($data, $vendorId);
        if ($ci < 0 || $vi < 0) { http_response_code(404); echo json_encode(['success'=>false,'message'=>'customer/vendor tidak ditemukan']); exit; }

        // find product
        $found = false;
        for ($p=0;$p<count($data['vendors'][$vi]['daftarProduk']);$p++) {
            if ($data['vendors'][$vi]['daftarProduk'][$p]['nama'] === $namaProduk) {
                $found = true; break;
            }
        }
        if (!$found) { http_response_code(404); echo json_encode(['success'=>false,'message'=>'produk tidak ditemukan']); exit; }

        $product = &$data['vendors'][$vi]['daftarProduk'][$p];
        $total = $product['harga'] * $qty;
        if ($data['customers'][$ci]['saldo'] < $total) { http_response_code(400); echo json_encode(['success'=>false,'message'=>'Saldo tidak cukup']); exit; }
        if ($product['stok'] < $qty) { http_response_code(400); echo json_encode(['success'=>false,'message'=>'Stok tidak cukup']); exit; }

        $data['customers'][$ci]['saldo'] -= $total;
        $product['stok'] -= $qty;
        saveData($data);
        echo json_encode(['success'=>true,'saldo'=>$data['customers'][$ci]['saldo']]); exit;
    }

    // --- Parkir: checkin ---
    if ($action === 'checkin') {
        $custId = $input['customerId'] ?? '';
        $plat = $input['plat'] ?? '';
        $slot = $input['slot'] ?? '';
        // ensure slot not occupied
        foreach ($data['parkir']['aktif'] as $t) {
            if ($t['slot'] === $slot && $t['status'] === 'AKTIF') { http_response_code(400); echo json_encode(['success'=>false,'message'=>'Slot sudah terisi']); exit; }
        }
        $now = time();
        $idTiket = 'TKT-' . $now . '-' . rand(100,999);
        $tiket = ['idTiket'=>$idTiket,'idPelanggan'=>$custId,'nomorPolisi'=>$plat,'waktuMasuk'=>$now,'waktuKeluar'=>0,'biaya'=>0,'status'=>'AKTIF','slot'=>$slot];
        $data['parkir']['aktif'][] = $tiket;
        saveData($data);
        echo json_encode(['success'=>true,'tiket'=>$tiket]); exit;
    }

    // --- Parkir: checkout ---
    if ($action === 'checkout') {
        $idTiket = $input['idTiket'] ?? '';
        $found = false;
        for ($i=0;$i<count($data['parkir']['aktif']);$i++) {
            if ($data['parkir']['aktif'][$i]['idTiket'] === $idTiket && $data['parkir']['aktif'][$i]['status'] === 'AKTIF') {
                $t = $data['parkir']['aktif'][$i];
                $t['waktuKeluar'] = time();
                $durasi = max(1, $t['waktuKeluar'] - $t['waktuMasuk']);
                $t['biaya'] = $durasi * 2000; // sama dengan C++
                $t['status'] = 'DIBAYAR';
                // remove from aktif and push to riwayat
                array_splice($data['parkir']['aktif'],$i,1);
                $data['parkir']['riwayat'][] = $t;
                // add loyalty
                $poin = intval($t['biaya'] / 1000);
                if ($poin>0) {
                    $foundP=false;
                    for ($j=0;$j<count($data['parkir']['poin']);$j++) {
                        if ($data['parkir']['poin'][$j]['nomorPolisi'] === $t['nomorPolisi']) { $data['parkir']['poin'][$j]['poin'] += $poin; $foundP=true; break; }
                    }
                    if (!$foundP) $data['parkir']['poin'][] = ['nomorPolisi'=>$t['nomorPolisi'],'poin'=>$poin];
                }
                saveData($data);
                echo json_encode(['success'=>true,'tiket'=>$t]); exit;
            }
        }
        http_response_code(404); echo json_encode(['success'=>false,'message'=>'Tiket tidak ditemukan']); exit;
    }

    // --- Parkir: cek poin ---
    if ($action === 'cek_poin') {
        $plat = $input['plat'] ?? '';
        foreach ($data['parkir']['poin'] as $p) if ($p['nomorPolisi'] === $plat) { echo json_encode(['success'=>true,'poin'=>$p]); exit; }
        echo json_encode(['success'=>true,'poin'=>['nomorPolisi'=>$plat,'poin'=>0]]); exit;
    }

    // Default
    http_response_code(400);
    echo json_encode(['success'=>false,'message'=>'Action tidak dikenali']);
    exit;
}
?>
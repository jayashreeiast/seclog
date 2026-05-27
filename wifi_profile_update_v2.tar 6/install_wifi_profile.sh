#!/bin/bash
 
PROFILE_ID="LMDD_WIFI_PROFILE"
PROFILE_SRC="wifi-profile.nmconnection"
PROFILE_DST="${PROFILE_ID}.nmconnection"
 
NM_DIR="/etc/NetworkManager/system-connections"
CERT_DIR="/etc/NetworkManager/certs"
 
fail() {
    echo "[OTA][ERROR] $1"
    exit 1
}
 
echo "[OTA] Installing certificate-based Wi-Fi profile..."
 
# -------------------------------
# Install certificates
# -------------------------------
mkdir -p "${CERT_DIR}" || fail "Cannot create cert directory"
 
cp certs/ca.pem     "${CERT_DIR}/" || fail "Failed to copy ca.pem"
cp certs/client.pem "${CERT_DIR}/" || fail "Failed to copy client.pem"
cp certs/client.key "${CERT_DIR}/" || fail "Failed to copy client.key"
 
chmod 600 "${CERT_DIR}"/* || fail "Failed to chmod certs"
chown root:root "${CERT_DIR}"/* || fail "Failed to chown certs"
 
# -------------------------------
# Install NM profile (CRITICAL FIX)
# -------------------------------
cp "${PROFILE_SRC}" "${NM_DIR}/${PROFILE_DST}" || fail "Failed to copy NM profile"
chmod 600 "${NM_DIR}/${PROFILE_DST}" || fail "Failed to chmod NM profile"
chown root:root "${NM_DIR}/${PROFILE_DST}" || fail "Failed to chown NM profile"
 
# -------------------------------
# Reload profiles
# -------------------------------
echo "[OTA] Reloading profiles"
nmcli con reload || fail "Failed to reload profiles"
 
sleep 4
 
# -------------------------------
# Verify registration
# -------------------------------
if ! nmcli -t -f NAME connection show | grep -qx "${PROFILE_ID}"; then
    fail "Wi-Fi profile not registered in NetworkManager"
fi
 
echo "[OTA] Wi-Fi profile registered"
 
# -------------------------------
# Activate connection
# -------------------------------
if nmcli connection up "${PROFILE_ID}"; then
    echo "[OTA] Wi-Fi connection activated"
else
    fail "Wi-Fi profile registered but activation failed"
fi
 
echo "[OTA] Certificate-based Wi-Fi update SUCCESS"
exit 0

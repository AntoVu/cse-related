import json
import logging
import http.server
import secrets
import os
import bcrypt
from datetime import datetime
from http import HTTPStatus
from pathlib import Path
from urllib.parse import parse_qs, urlparse

# AES encryption from cryptography library
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

# Uploads has the encrypted files
# Keys has the AES key
# Users is for the bonus with encrypted passwords
BASE_DIR = Path(__file__).parent
UPLOAD_DIR = BASE_DIR / "uploads"
KEY_DIR = BASE_DIR / "keys"
USERS_FILE = BASE_DIR / "users.json"

# Create the upload and key directories if they do not already exist
for d in (UPLOAD_DIR, KEY_DIR):
    d.mkdir(exist_ok=True)

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger("securestore")

# Generate 256 bit key once
AES_KEY_PATH = KEY_DIR / "aes.key"

def _load_or_generate_aes_key():
    """Load the AES key from disk, or generate and save a new one."""
    if AES_KEY_PATH.exists():
        return AES_KEY_PATH.read_bytes()
    key = secrets.token_bytes(32) # 32 bytes = 256 bits
    AES_KEY_PATH.write_bytes(key)
    log.info("New AES-256 key generated and saved to %s", AES_KEY_PATH)
    return key

AES_KEY = _load_or_generate_aes_key()

# Encrypt raw data using AES encryption
def encrypt_bytes(plaintext: bytes) -> bytes:
    """
    Encrypt raw bytes with AES-256-CBC.
    Layout of returned value:  [16-byte IV] + [ciphertext with PKCS#7 padding]
    """
    iv = secrets.token_bytes(16) # Initialization vector
    pad_len = 16 - (len(plaintext) % 16) # Padding to next block
    plaintext += bytes([pad_len] * pad_len) # Pad value = number of pad bytes
    cipher = Cipher(algorithms.AES(AES_KEY), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    return iv + encryptor.update(plaintext) + encryptor.finalize()

# Reverses the encryption (decrypts the data)
def decrypt_bytes(data: bytes) -> bytes:
    """
    Decrypt AES-256-CBC ciphertext produced by encrypt_bytes().
    Strips the leading IV, decrypts, then removes PKCS#7 padding.
    """
    iv = data[:16] # Pull initialization vector
    ciphertext = data[16:]
    cipher = Cipher(algorithms.AES(AES_KEY), modes.CBC(iv), backend=default_backend()) # Create cipher
    decryptor = cipher.decryptor()
    plaintext = decryptor.update(ciphertext) + decryptor.finalize() # Decrypt with cipher
    pad_len = plaintext[-1] # Prepare to remove padding
    return plaintext[:-pad_len]

# User login validation
def authenticate_user(username, password):
    """Return True if username exists and password matches the stored bcrypt hash."""
    if not USERS_FILE.exists():
        return False
    users = json.loads(USERS_FILE.read_text())
    if username not in users: # If no username matches
        return False
    return bcrypt.checkpw(password.encode(), users[username].encode()) # Check if password = hashed password

# Add a new user
def register_user(username, password):
    """
    Add a new user to users.json.
    Returns False if the username is already taken, True on success.
    """
    users = json.loads(USERS_FILE.read_text()) if USERS_FILE.exists() else {}
    if username in users: # User already exists
        return False
    users[username] = bcrypt.hashpw(password.encode(), bcrypt.gensalt()).decode() # Hash the password
    USERS_FILE.write_text(json.dumps(users))
    return True

SESSIONS = {}

# HTTP request handler
class SecureStoreHandler(http.server.BaseHTTPRequestHandler):
    # JSON helper response function
    def _json(self, data, status=HTTPStatus.OK):
        body = json.dumps(data).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", len(body))
        self.end_headers()
        self.wfile.write(body)

    # Authentication header helper
    def _get_authenticated_user(self):
        auth_header = self.headers.get('Authorization', '')
        if 'Bearer ' not in auth_header:
            return None
        token = auth_header.replace('Bearer ', '')
        return SESSIONS.get(token) # None if token not found

    def do_GET(self):
        parsed = urlparse(self.path)
        path = parsed.path

        # Use the html file
        if path in ("/", "/index.html"):
            content = (BASE_DIR / "client.html").read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "text/html")
            self.end_headers()
            self.wfile.write(content)
            return

        # Ensures user is not allowed in if not logged in
        user = self._get_authenticated_user()
        if user is None:
            self._json({"ok": False, "message": "Unauthorized"}, status=HTTPStatus.UNAUTHORIZED)
            return

        # List of files
        if path == "/api/list":
            files = []
            # Look for .enc files in the upload directory
            for f in UPLOAD_DIR.glob("*.enc"):
                stat = f.stat()
                files.append({
                    "name": f.name.replace('.enc', ''), # Name without .enc
                    "size_enc": stat.st_size, # Size of the encrypted file
                    "uploaded_at": datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M") # Timestamp it was uploaded
                })
            self._json({"ok": True, "files": files})

        # Download request for a file
        elif path == "/api/download":
            query = parse_qs(parsed.query)
            name = query.get("name", [None])[0]
            if not name:
                self._json({"ok": False, "message": "Missing filename"}, status=HTTPStatus.BAD_REQUEST)
                return
            f_path = UPLOAD_DIR / f"{name}.enc"
            if not f_path.exists():
                self._json({"ok": False, "message": "File not found"}, status=HTTPStatus.NOT_FOUND)
                return
            # Decrypt the file before sending
            plaintext = decrypt_bytes(f_path.read_bytes())
            self.send_response(200)
            self.send_header("Content-Type", "application/octet-stream")
            # Tells the browser to save the file with the original name
            self.send_header("Content-Disposition", f'attachment; filename="{name}"')
            self.end_headers()
            self.wfile.write(plaintext)

    def do_POST(self):
        path = urlparse(self.path).path
        length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(length)

        # Give session token on login
        if path == "/api/login":
            data = json.loads(body)
            if authenticate_user(data['username'], data['password']):
                # Generate a random token
                token = secrets.token_hex(16)
                SESSIONS[token] = data['username'] # Add this session to SESSIONS
                self._json({"ok": True, "token": token, "username": data['username']})
            else:
                self._json({"ok": False, "message": "Invalid username or password"}, 401)

        # Create a new account
        elif path == "/api/register":
            data = json.loads(body)
            if register_user(data['username'], data['password']):
                self._json({"ok": True})
            else:
                self._json({"ok": False, "message": "Username already taken"}, 400)

        # End the current session (logout)
        elif path == "/api/logout":
            auth_header = self.headers.get('Authorization', '')
            if 'Bearer ' in auth_header:
                token = auth_header.replace('Bearer ', '')
                SESSIONS.pop(token, None) # Remove token
            self._json({"ok": True})

        # Encrypt the upload
        elif path == "/api/upload":
            # Ensure uploader is valid session
            user = self._get_authenticated_user()
            if user is None:
                self._json({"ok": False, "message": "Unauthorized"}, status=HTTPStatus.UNAUTHORIZED)
                return

            # Send original filename
            from urllib.parse import unquote
            filename = unquote(self.headers.get("X-Filename", "file.bin"))

            # Encrypt the raw bytes before writing
            ciphertext = encrypt_bytes(body)
            (UPLOAD_DIR / f"{filename}.enc").write_bytes(ciphertext)
            log.info("User '%s' uploaded and encrypted: %s", user, filename)
            self._json({"ok": True})

    # Suppress the default request log line
    def log_message(self, format, *args):
        log.info("%s - %s", self.address_string(), format % args)

def main():
    server = http.server.HTTPServer(("127.0.0.1", 8000), SecureStoreHandler)
    print("Server started at http://localhost:8000")
    server.serve_forever()

if __name__ == "__main__":
    main()
import gzip
import base64

def encode_html(input_file, output_file):
    with open(input_file, "rb") as f:
        compressed_data = gzip.compress(f.read())
    
    array_data = ",".join(str(b) for b in compressed_data)
    
    with open(output_file, "w") as f:
        f.write("const uint8_t ELEGANT_HTML[] PROGMEM = { " + array_data + " };")
    
    print("Encoding complete! Output saved in", output_file)

def decode_html(input_file, output_file):
    with open(input_file, "r") as f:
        data = f.read()

    # Ambil bagian dalam kurung kurawal dan bersihkan spasi atau newline
    raw_bytes = data.split("{")[1].split("}")[0].strip()
    byte_array = bytes(map(int, raw_bytes.replace("\n", "").split(",")))

    decompressed_data = gzip.decompress(byte_array)

    with open(output_file, "wb") as f:
        f.write(decompressed_data)

    print("Decoding complete! Output saved in", output_file)

# Contoh penggunaan:
encode_html("decoded.html", "elop.cpp")
# decode_html("code.cpp", "decoded.html")
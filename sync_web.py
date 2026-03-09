import re
import os

# --- Configuración de Archivos ---
# Usamos rutas relativas para que funcione en cualquier PC
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
main_file = os.path.join(BASE_DIR, "main.ino")
index_file = os.path.join(BASE_DIR, "index.html")

def sync_html_to_ino():
    """Lee index.html y actualiza el bloque INDEX_HTML en main.ino"""
    if not os.path.exists(index_file):
        print(f"❌ ERROR: No se encuentra {index_file}")
        return

    with open(index_file, 'r', encoding='utf-8') as f:
        html_content = f.read().strip()

    if not os.path.exists(main_file):
        print(f"❌ ERROR: No se encuentra {main_file}")
        return

    with open(main_file, 'r', encoding='utf-8') as f:
        ino_content = f.read()

    # Expresión regular para encontrar el bloque R"=====( ... )=====";
    pattern = r'(const char INDEX_HTML\[\] PROGMEM = R"=====\(\n)(.*?)(\n\)=====";)'
    
    # Reemplazamos el contenido manteniendo los delimitadores
    new_ino_content = re.sub(pattern, rf'\1{html_content}\3', ino_content, flags=re.DOTALL)

    if new_ino_content != ino_content:
        with open(main_file, 'w', encoding='utf-8') as f:
            f.write(new_ino_content)
        print("✅ ÉXITO: main.ino ha sido actualizado con el contenido de index.html")
    else:
        print("ℹ️ INFO: No hay cambios detectados o no se encontró el bloque INDEX_HTML en main.ino")

if __name__ == "__main__":
    sync_html_to_ino()

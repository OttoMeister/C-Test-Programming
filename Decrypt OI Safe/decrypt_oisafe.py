#!/usr/bin/env python3
# python3 decrypt_oisafe.py oisafe.xml

import subprocess
import xml.etree.ElementTree as ET
import csv
import sys
import getpass
import re
import os

def run_decryptor(password, salt, masterkey, *fields):
    """Führt OISafeDecryptor aus mit allen Feldern auf einmal"""
    args = ['./OISafeDecryptor', password, salt, masterkey] + list(fields)
    
    try:
        result = subprocess.run(args, capture_output=True, text=True)
        return result.stdout, result.returncode
    except FileNotFoundError:
        print("Fehler: './OISafeDecryptor' nicht gefunden. Bitte sicherstellen, dass die Datei im selben Ordner liegt.")
        sys.exit(1)

def parse_output(output):
    """Liest Master Key und entschlüsselte Felder aus der C++ Ausgabe"""
    master = None
    fields = []
    
    for line in output.split('\n'):
        if line.startswith('Master:'):
            master = line.split('Master:', 1)[1].strip()
            break
    
    field_pattern = r'Field: (.*?)(?=\nField: |\nMaster: |$)'
    matches = re.findall(field_pattern, output, re.DOTALL)
    
    for match in matches:
        fields.append(match)
    
    return master, fields

if len(sys.argv) > 1:
    xml_file = sys.argv[1]
else:
    xml_file = 'oisafe.xml'

print(f"Lese XML-Datei: {xml_file}")

try:
    tree = ET.parse(xml_file)
    root = tree.getroot()
except Exception as e:
    print(f"Fehler beim Lesen der XML: {e}")
    sys.exit(1)

salt = root.find('Salt').text
masterkey_enc = root.find('MasterKey').text
backup_date = root.get('date', '')

print(f"  - Backup Datum gefunden: {backup_date}")
print(f"  - Salt: {salt}")

master_pwd = getpass.getpass("Bitte OI Safe Master-Passwort eingeben: ")

print("\nEntschlüssele Einträge...")

all_rows = []
entry_count = 0

for category in root.findall('.//Category'):
    cat_enc = category.get('name')
    
    for entry in category.findall('Entry'):
        entry_count += 1
        row_id = entry.find('RowID').text
        
        desc_enc = entry.find('Description').text
        web_enc = entry.find('Website').text
        user_enc = entry.find('Username').text
        pass_enc = entry.find('Password').text
        note_enc = entry.find('Note').text
        
        fields_to_decrypt = [cat_enc]
        field_indices = {'category': 0}
        
        placeholder = 'd21509bb48e16734a7ba78a40946a67f'
        current_idx = 1
        
        def add_field(val, name, idx_counter):
            if val and val != placeholder:
                fields_to_decrypt.append(val)
                field_indices[name] = idx_counter
                return idx_counter + 1
            return idx_counter

        current_idx = add_field(note_enc, 'note', current_idx)
        current_idx = add_field(desc_enc, 'desc', current_idx)
        current_idx = add_field(web_enc, 'web', current_idx)
        current_idx = add_field(user_enc, 'user', current_idx)
        current_idx = add_field(pass_enc, 'pass', current_idx)
        
        output, returncode = run_decryptor(master_pwd, salt, masterkey_enc, *fields_to_decrypt)
        
        if returncode != 0:
            print(f"✗ Fehler bei Eintrag ID {row_id}")
            continue
        
        master, decrypted_fields = parse_output(output)
        
        def get_val(name):
            if name in field_indices and len(decrypted_fields) > field_indices[name]:
                return decrypted_fields[field_indices[name]]
            return ""

        cat_dec = get_val('category')
        note_dec = get_val('note')
        desc_dec = get_val('desc')
        web_dec = get_val('web')
        user_dec = get_val('user')
        pass_dec = get_val('pass')
        
        last_edited = backup_date
        
        all_rows.append({
            'row_id': int(row_id),
            'data': [cat_dec, desc_dec, web_dec, user_dec, pass_dec, note_dec, last_edited]
        })
        
        desc_short = desc_dec.replace('\n', ' ').strip()[:30]
        print(f"[{entry_count}] ID:{row_id} {cat_dec}: {desc_short}")


print(f"\nSchreibe CSV-Datei...")

csv_output = xml_file.replace('.xml', '.csv')

with open(csv_output, 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f, quoting=csv.QUOTE_ALL)
    writer.writerow(['Category', 'Description', 'Website', 'Username', 'Password', 'Notes', 'Last edited'])
    
    for row in all_rows:
        writer.writerow(row['data'])

print(f"✓ Fertig! '{csv_output}' wurde erstellt.")

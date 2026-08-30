mod data;

use data::{CharInfo, DataStore};
use tauri::{Manager, State};

struct AppState {
    store: DataStore,
}

#[tauri::command]
fn query_chars(input: String, state: State<AppState>) -> Vec<CharInfo> {
    let results = state.store.query(&input);
    eprintln!("query '{}' -> {} results", input, results.len());
    results
}

#[tauri::command]
fn get_char_count(state: State<AppState>) -> usize {
    state.store.char_count()
}

#[tauri::command]
fn get_image_base64(character: String, state: State<AppState>) -> Option<String> {
    let info = state.store.char_map.get(&character)?;
    if !info.has_image || info.image_path.is_empty() {
        return None;
    }
    eprintln!("get_image_base64: character={}, path={}", character, info.image_path);
    let bytes = std::fs::read(&info.image_path).ok();
    match bytes {
        Some(b) => {
            eprintln!("read {} bytes", b.len());
            Some(format!("data:image/gif;base64,{}", base64_encode(&b)))
        }
        None => {
            eprintln!("failed to read file");
            None
        }
    }
}

fn base64_encode(data: &[u8]) -> String {
    const CHARS: &[u8] = b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    let mut result = String::with_capacity((data.len() + 2) / 3 * 4);
    for chunk in data.chunks(3) {
        let b0 = chunk[0] as u32;
        let b1 = if chunk.len() > 1 { chunk[1] as u32 } else { 0 };
        let b2 = if chunk.len() > 2 { chunk[2] as u32 } else { 0 };
        let n = (b0 << 16) | (b1 << 8) | b2;
        result.push(CHARS[((n >> 18) & 0x3F) as usize] as char);
        result.push(CHARS[((n >> 12) & 0x3F) as usize] as char);
        if chunk.len() > 1 {
            result.push(CHARS[((n >> 6) & 0x3F) as usize] as char);
        } else {
            result.push('=');
        }
        if chunk.len() > 2 {
            result.push(CHARS[(n & 0x3F) as usize] as char);
        } else {
            result.push('=');
        }
    }
    result
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_fs::init())
        .setup(|app| {
            let assets_dir = if cfg!(debug_assertions) {
                std::env::current_dir()
                    .unwrap()
                    .parent()
                    .unwrap()
                    .join("resources")
            } else {
                let resource_dir = app.path().resource_dir().unwrap();
                resource_dir.join("_up_").join("resources")
            };

            let store = DataStore::new(&assets_dir).expect("数据加载失败");
            eprintln!("已加载 {} 个汉字", store.char_count());

            app.manage(AppState { store });

            if cfg!(debug_assertions) {
                app.handle().plugin(
                    tauri_plugin_log::Builder::default()
                        .level(log::LevelFilter::Info)
                        .build(),
                )?;
            }
            Ok(())
        })
        .invoke_handler(tauri::generate_handler![query_chars, get_char_count, get_image_base64])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

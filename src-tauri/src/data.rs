use serde::Serialize;
use std::collections::{HashMap, HashSet};
use std::fs;
use std::path::Path;

#[derive(Clone, Serialize, Debug)]
pub struct CharInfo {
    pub character: String,
    pub wubi_code: String,
    pub simple_codes: Vec<String>,
    pub pinyin: String,
    pub strokes: String,
    pub segments: String,
    pub has_image: bool,
    pub image_path: String,
}

pub struct DataStore {
    pub char_map: HashMap<String, CharInfo>,
}

impl DataStore {
    pub fn new(assets_dir: &Path) -> Result<Self, String> {
        let wubi_path = assets_dir.join("data").join("data-wubi-v86.tsv");
        let char_path = assets_dir.join("data").join("data-chars.tsv");
        let images_dir = assets_dir.join("images").join("char");

        let mut char_map = Self::load_wubi_data(&wubi_path)?;
        Self::load_char_data(&char_path, &mut char_map)?;
        let available_images = Self::scan_images(&images_dir)?;
        Self::filter_and_enrich(&mut char_map, &available_images, &images_dir);

        Ok(DataStore { char_map })
    }

    fn load_wubi_data(path: &Path) -> Result<HashMap<String, CharInfo>, String> {
        let content = fs::read_to_string(path)
            .map_err(|e| format!("无法打开五笔数据文件 {}: {}", path.display(), e))?;

        let mut map: HashMap<String, CharInfo> = HashMap::new();

        for line in content.lines().skip(1) {
            let cells: Vec<&str> = line.split('\t').collect();
            if cells.is_empty() || cells[0].is_empty() {
                continue;
            }

            let character = cells[0].to_string();
            let wubi_code = if cells.len() > 1 { cells[1].to_string() } else { String::new() };

            let mut simple_codes = Vec::new();
            if cells.len() > 2 && !cells[2].is_empty() {
                simple_codes.push(cells[2].to_string());
            }
            if cells.len() > 3 && !cells[3].is_empty() {
                simple_codes.push(cells[3].to_string());
            }

            let segments = if cells.len() > 9 {
                cells[9].to_string()
            } else if cells.len() > 8 {
                cells[8].to_string()
            } else {
                String::new()
            };

            if let Some(existing) = map.get_mut(&character) {
                existing.wubi_code = wubi_code;
                existing.simple_codes = simple_codes;
                existing.segments = segments;
            } else {
                map.insert(
                    character.clone(),
                    CharInfo {
                        character,
                        wubi_code,
                        simple_codes,
                        pinyin: String::new(),
                        strokes: String::new(),
                        segments,
                        has_image: false,
                        image_path: String::new(),
                    },
                );
            }
        }

        Ok(map)
    }

    fn load_char_data(
        path: &Path,
        map: &mut HashMap<String, CharInfo>,
    ) -> Result<(), String> {
        let content = fs::read_to_string(path)
            .map_err(|e| format!("无法打开汉字信息文件 {}: {}", path.display(), e))?;

        for line in content.lines().skip(1) {
            let cells: Vec<&str> = line.split('\t').collect();
            if cells.len() < 2 || cells[1].is_empty() {
                continue;
            }

            let character = cells[1].to_string();
            let strokes = if cells.len() > 5 { cells[5].to_string() } else { String::new() };
            let pinyin = if cells.len() > 6 { cells[6].to_string() } else { String::new() };

            if let Some(existing) = map.get_mut(&character) {
                existing.pinyin = pinyin;
                existing.strokes = strokes;
            } else {
                map.insert(
                    character.clone(),
                    CharInfo {
                        character,
                        wubi_code: String::new(),
                        simple_codes: Vec::new(),
                        pinyin,
                        strokes,
                        segments: String::new(),
                        has_image: false,
                        image_path: String::new(),
                    },
                );
            }
        }

        Ok(())
    }

    fn scan_images(dir: &Path) -> Result<HashSet<String>, String> {
        let mut set = HashSet::new();

        if !dir.exists() {
            return Err(format!("图片目录不存在: {}", dir.display()));
        }

        let entries = fs::read_dir(dir)
            .map_err(|e| format!("无法读取图片目录 {}: {}", dir.display(), e))?;

        for entry in entries {
            let entry = entry.map_err(|e| e.to_string())?;
            let path = entry.path();
            if path.is_file() {
                if let Some(name) = path.file_stem() {
                    if let Some(ext) = path.extension() {
                        if ext == "gif" {
                            set.insert(name.to_string_lossy().to_string());
                        }
                    }
                }
            }
        }

        Ok(set)
    }

    fn filter_and_enrich(
        map: &mut HashMap<String, CharInfo>,
        available: &HashSet<String>,
        images_dir: &Path,
    ) {
        map.retain(|character, info| {
            if available.contains(character) {
                info.has_image = true;
                info.image_path = images_dir
                    .join(format!("{}.gif", character))
                    .to_string_lossy()
                    .to_string();
                true
            } else {
                false
            }
        });
    }

    pub fn query(&self, input: &str) -> Vec<CharInfo> {
        let mut seen = HashSet::new();
        let mut results = Vec::new();

        for ch in input.chars() {
            if !Self::is_chinese_char(ch) {
                continue;
            }
            let s = ch.to_string();
            if seen.contains(&s) {
                continue;
            }
            seen.insert(s.clone());

            if let Some(info) = self.char_map.get(&s) {
                results.push(info.clone());
            }
        }

        results
    }

    fn is_chinese_char(c: char) -> bool {
        let cp = c as u32;
        (0x4E00..=0x9FFF).contains(&cp)
            || (0x3400..=0x4DBF).contains(&cp)
            || (0xF900..=0xFAFF).contains(&cp)
    }

    pub fn char_count(&self) -> usize {
        self.char_map.len()
    }
}

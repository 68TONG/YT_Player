SELECT F_Info.Id
  FROM FileInfo AS F_Info
 WHERE :api_data = json_extract(F_Info.autoUpdateData, :json_path);

INSERT INTO FileInfo (
                         filePath,
                         fileTitle,
                         fileTag,
                         createDate,
                         creatorId,
                         autoUpdateData
                     )
                     VALUES (
                         ?,
                         ?,
                         ?,
                         ?,
                         ?,
                         json_pretty(?) 
                     );

UPDATE CreatorInfo
   SET autoUpdateData = json_pretty(json_set(autoUpdateData, :json_path, :api_data) ) 
 WHERE Id = :info_id;

UPDATE CreatorInfo
   SET autoUpdateData = json_pretty(json_set(autoUpdateData, :json_path, (
                                                 SELECT MAX(createDate) 
                                                   FROM FileInfo
                                                  WHERE creatorId = :info_id
                                             )
                                    ) ) 
 WHERE Id = :info_id;

UPDATE CreatorInfo
   SET autoUpdateData = json_pretty(json_set(autoUpdateData, :json_path, :api_data) ) 
 WHERE Id = :info_id AND
       NOT EXISTS (
               SELECT 1
                 FROM json_each(autoUpdateData, :json_path_re_duplicate) 
                WHERE json_each.value = :api_data
           );

UPDATE FileInfo
   SET createDate = datetime(createDate);
   
UPDATE FileInfo
   SET autoUpdateData = json_pretty(json_set(autoUpdateData, '$.pixiv.createDate', createDate) );
   
UPDATE FileInfo
   SET autoUpdateData = json_pretty(json_set(autoUpdateData, '$.pixiv.updateDate', autoUpdateData->>'$.pixiv.date') );
   
UPDATE FileInfo
   SET autoUpdateData = json_pretty(json_remove(autoUpdateData, '$.pixiv.date') );

% filepath: h:\ROBOT_TER\Communication_robot_pc\plot_combined.m
% Script MATLAB pour tracer les mouvements du robot et la carte locale

% Noms des fichiers
position_filename = 'position_robot.txt';
map_filename = 'map_local.txt';

% Initialiser les dates de modification précédentes
last_modified_position = "";
last_modified_map = "";

% Variables pour stocker les données du robot et de la carte
x_robot = [];
y_robot = [];
robot_handle = []; % Handle pour le carré rouge
map_handle = []; % Handle pour les points de la carte

% Boucle infinie pour surveiller les mises à jour des fichiers
while true
    % Vérifier si le fichier position_robot.txt existe
    if isfile(position_filename)
        % Obtenir la date de modification du fichier position_robot.txt
        file_info_position = dir(position_filename);
        current_modified_position = file_info_position.date;
        
        % Vérifier si le fichier position_robot.txt a été modifié
        if ~strcmp(last_modified_position, current_modified_position)
            % Mettre à jour la date de modification précédente
            last_modified_position = current_modified_position;
            
            % Lire les données du fichier position_robot.txt
            data_position = load(position_filename); % Charger les données (format attendu : deux colonnes [x_robot, y_robot])
            
            % Vérifier si les données sont valides
            if ~isempty(data_position)
                % Extraire les coordonnées X et Y
                x_robot = data_position(:, 1);
                y_robot = data_position(:, 2);
                
                % Tracer les positions du robot progressivement
                figure(1); % Utiliser la même figure
                clf; % Effacer le contenu précédent
                hold on;
                grid on;
                axis equal;
                
                % Tracer les points intermédiaires un par un
                for i = 1:length(x_robot)
                    % Relier les points intermédiaires par des pointillés verts
                    if i > 1
                        plot(x_robot(1:i), y_robot(1:i), 'g--', 'LineWidth', 1.5); % Ligne pointillée verte
                    end
                    
                    % Tracer le point actuel
                    plot(x_robot(i), y_robot(i), 'bo', 'MarkerSize', 5, 'MarkerFaceColor', 'b'); % Point bleu
                    
                    % Supprimer le carré précédent s'il existe
                    if ~isempty(robot_handle)
                        delete(robot_handle);
                    end
                    
                    % Tracer la position actuelle du robot comme un carré rouge
                    robot_handle = rectangle('Position', [x_robot(i)-5, y_robot(i)-5, 10, 10], 'EdgeColor', 'r', 'LineWidth', 2); % Carré rouge
                    
                    % Ajouter des labels et un titre
                    xlabel('X (cm)');
                    ylabel('Y (cm)');
                    title('Trajectoire du robot et carte locale');
                    
                    % Mettre à jour le graphique
                    drawnow;
                    
                    % Pause de 100 ms entre chaque point
                    pause(0.3);
                end
            else
                disp('Le fichier position_robot.txt est vide ou contient des données invalides.');
            end
        end
    else
        disp('Fichier position_robot.txt introuvable.');
    end
    
    % Vérifier si le fichier map_local.txt existe
    if isfile(map_filename)
        % Obtenir la date de modification du fichier map_local.txt
        file_info_map = dir(map_filename);
        current_modified_map = file_info_map.date;
        
        % Vérifier si le fichier map_local.txt a été modifié
        if ~strcmp(last_modified_map, current_modified_map)
            % Mettre à jour la date de modification précédente
            last_modified_map = current_modified_map;
            
            % Lire les données du fichier map_local.txt
            data_map = load(map_filename); % Charger les données (format attendu : deux colonnes [Xp, Yp])
            
            % Vérifier si les données sont valides
            if ~isempty(data_map)
                % Extraire les coordonnées X et Y
                Xp = data_map(:, 1);
                Yp = data_map(:, 2);
                
                % Tracer la carte locale
                if ~isempty(map_handle)
                    delete(map_handle); % Supprimer les points précédents de la carte
                end
                map_handle = plot(Xp, Yp, 'bo', 'MarkerSize', 5, 'MarkerFaceColor', 'b'); % Points en bleu
                
                % Conserver la trajectoire du robot et le carré rouge
                if ~isempty(x_robot) && ~isempty(y_robot)
                    plot(x_robot, y_robot, 'g--', 'LineWidth', 1.5); % Ligne pointillée verte
                    if ~isempty(robot_handle)
                        delete(robot_handle);
                    end
                    robot_handle = rectangle('Position', [x_robot(end)-5, y_robot(end)-5, 10, 10], 'EdgeColor', 'r', 'LineWidth', 2); % Carré rouge
                end
                
                % Ajouter des labels et un titre
                xlabel('X (cm)');
                ylabel('Y (cm)');
                title('Trajectoire du robot et carte locale');
                
                % Mettre à jour le graphique
                drawnow;
            else
                disp('Le fichier map_local.txt est vide ou contient des données invalides.');
            end
        end
    else
        disp('Fichier map_local.txt introuvable.');
    end
    
    % Pause pour éviter une surcharge du processeur
    pause(1); % Vérifier les mises à jour toutes les secondes
end
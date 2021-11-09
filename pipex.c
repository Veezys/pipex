/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: breadyma <breadyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/05 16:54:33 by breadyma          #+#    #+#             */
/*   Updated: 2021/11/09 14:42:18 by breadyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*ft_cmd_find(char **path, char *command)
{
	int		i;
	char	*dir;
	char	*dircmd;
	int		valid;

	i = 0;
	while (path[i])
	{
		dir = ft_strjoin(path[i], "/");
		dircmd = ft_strjoin(dir, command);
		free(dir);
		valid = access(dircmd, F_OK);
		if (valid < 0)
			free(dircmd);
		else
			return (dircmd);
		i++;
	}
	return (NULL);
}

char	**ft_find_patch(char **env)
{
	char	**patch;
	int		i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
		{
			patch = ft_split(env[i] + 5, ':');
			return (patch);
		}
		i++;
	}
	perror("path not found");
	exit(0);
}

void	ft_child(int fd[2], char **argv, char **env)
{
	int		infile;
	char	**patch;
	char	**command;
	char	*waycmd;

	infile = open(argv[1], O_RDONLY);
	if (infile < 0)
	{
		perror("file not found");
		exit(0);
	}
	dup2(infile, 0);
	dup2(fd[1], 1);
	close(fd[0]);
	patch = ft_find_patch(env);
	command = ft_split(argv[2], ' ');
	waycmd = ft_cmd_find(patch, command[0]);
	if (!waycmd)
		perror("command not found");
	execve(waycmd, command, env);
}

void	ft_parent(int fd[2], char **argv, char **env)
{
	int		outfile;
	char	**patch;
	char	**command;
	char	*waycmd;

	wait(0);
	outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (outfile < 0)
	{
		perror("file not found");
		exit(0);
	}
	dup2(fd[0], 0);
	dup2(outfile, 1);
	close(fd[1]);
	patch = ft_find_patch(env);
	command = ft_split(argv[3], ' ');
	waycmd = ft_cmd_find(patch, command[0]);
	if (!waycmd)
		perror("command not found");
	execve(waycmd, command, env);
}

int	main(int argc, char **argv, char **env)
{
	int		fd[2];
	pid_t	work;

	if (argc != 5)
		return (0);
	if (pipe(fd) == -1)
		return (0);
	work = fork();
	if (work == -1)
	{
		perror("processing error");
		return (0);
	}
	if (work == 0)
		ft_child(fd, argv, env);
	else
		ft_parent(fd, argv, env);
	return (0);
}
